#include <iostream>
#include <string>
#include <vector>
#include "instruction.hpp"
#include "bytecodes.hpp"
#include "oclVM.hpp"

#include <stdio.h>
#include <stdlib.h>

using namespace std;

OCLVM::OCLVM(vector<int> code, int mainByteCodeIndex) {
    this->code = code;
    this->codeSize = code.size();
    this->ip = mainByteCodeIndex;
    this->ins = createAllInstructions();
}

OCLVM::~OCLVM() {
    if (vmAllocated) {
        this->stack.clear();
        this->data.clear();
    }
}

void OCLVM::setPlatform(int numPlatform) {
    this->platformNumber = numPlatform;
}

int OCLVM::readBinaryFile(unsigned char **output, size_t *size, const char *name) {
    FILE* fp = fopen(name, "rb");
    if (!fp) {
        return -1; 
    }
    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    *output = (unsigned char *)malloc(*size);
    if (!*output) {
        fclose(fp);
        return -1; 
    }
    fread(*output, *size, 1, fp);
    fclose(fp);
    return 0;
}


int OCLVM::initOpenCL(string kernelFilename, bool loadBinary) {
    cl_int status;	
	cl_uint numPlatforms = 0;

	status = clGetPlatformIDs(0, NULL, &numPlatforms);

	if (numPlatforms == 0) {
		cout << "No platform detected" << endl;
		return -1;
	}

	platforms = (cl_platform_id*) malloc(numPlatforms*sizeof(cl_platform_id));
	if (platforms == NULL) {
		cout << "malloc platform_id failed" << endl;
		return -1;
	}
	
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);
	if (status != CL_SUCCESS) {
		cout << "clGetPlatformIDs failed" << endl;
		return -1;
	}	

	cout << numPlatforms <<  " has been detected" << endl;
	for (int i = 0; i < numPlatforms; i++) {
		char buf[10000];
		cout << "Platform: " << i << endl;
		status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(buf), buf, NULL);
		if (i == platformNumber) {
			platformName += buf;
		}
		cout << "\tVendor: " << buf << endl;
		status = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(buf), buf, NULL);
	}

	cl_uint numDevices = 0;

	cl_platform_id platform = platforms[platformNumber];
	std::cout << "Using platform: " << platformNumber << " --> " << platformName << std::endl;

	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
	
	if (status != CL_SUCCESS) {
		cout << "[WARNING] Using CPU, no GPU available" << endl;
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);
		devices = (cl_device_id*) malloc(numDevices*sizeof(cl_device_id));
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);
	} else {
		devices = (cl_device_id*) malloc(numDevices*sizeof(cl_device_id));
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
	}
	
	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
	
	commandQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &status);	
	if (status != CL_SUCCESS || commandQueue == NULL) {
		cout << "Error in create command" << endl;
		return -1;
	}

    if (loadBinary) {
        unsigned char* binary = NULL;
        size_t program_size = 0;
        readBinaryFile(&binary, &program_size, kernelFilename.c_str());
 
        cl_device_id dev = devices[0];
        program = clCreateProgramWithBinary(context, 1, &dev, &program_size, (const unsigned char **) &binary,  NULL, &status);
        if (status != CL_SUCCESS) {
            cout << "Error in clCreateProgramWithBinary" << endl;
        }
 
        cl_int buildErr = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);
        free(binary);
 
        kernel1 = clCreateKernel(program, "interpreter", &status);
        if (status != CL_SUCCESS) {
            cout << "Error in clCreateKernel" << endl;
            abort();
        }

    } else { 
	    const char *sourceFile = kernelFilename.c_str();
	    source = readSource(sourceFile);
	    program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &status);

	    cl_int buildErr;
	    buildErr = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);
	    kernel1 = clCreateKernel(program, "interpreter", &status);
	    if (status != CL_SUCCESS) {
		    cout << "Error in clCreateKernel" << endl;
		    abort();	
	    }
    }
}

void OCLVM::runInterpreter() {

    this->buffer = new char[100000];

    cout <<" CODE SIZE: " << this->codeSize << endl;

    // Create all buffers
    cl_int status;
 	cl_mem d_code = clCreateBuffer(context, CL_MEM_READ_ONLY, codeSize * sizeof(int), NULL, &status);
    if (status != CL_SUCCESS) {
        cout << "Error in clCreateBuffer: " << status << endl;
    }
    cl_mem d_stack = clCreateBuffer(context, CL_MEM_READ_WRITE, stackSize * sizeof(int), NULL, &status);
    cl_mem d_data = clCreateBuffer(context, CL_MEM_READ_WRITE, dataSize * sizeof(int), NULL, &status);
    cl_mem d_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, 100000 * sizeof(char), NULL, &status);
    
    // Copy code from HOST->DEVICE
    status = clEnqueueWriteBuffer(commandQueue, d_code, CL_TRUE, 0, codeSize * sizeof(int), code.data(), 0, NULL, &writeEvent);
    if (status != CL_SUCCESS) {
        cout << "Error in clEnqueueWriteBuffer" << endl;
    }

    // Push Arguments
	status  = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &d_code);
    status |= clSetKernelArg(kernel1, 1, sizeof(cl_mem), &d_stack);
    status |= clSetKernelArg(kernel1, 2, sizeof(cl_mem), &d_data);
    status |= clSetKernelArg(kernel1, 3, sizeof(cl_mem), &d_buffer);
	status |= clSetKernelArg(kernel1, 4, sizeof(cl_int), &codeSize);
    status |= clSetKernelArg(kernel1, 5, sizeof(cl_int), &ip);
    status |= clSetKernelArg(kernel1, 6, sizeof(cl_int), &fp);
    status |= clSetKernelArg(kernel1, 7, sizeof(cl_int), &sp);
    status |= clSetKernelArg(kernel1, 8, sizeof(cl_int), &trace);
    if (status != CL_SUCCESS) {
		cout << "Error in clSetKernelArgs" << endl;
	}

    // Launch Kernel
    size_t globalWorkSize[] = {1};
    status = clEnqueueNDRangeKernel(commandQueue, kernel1, 1, NULL, globalWorkSize, NULL, 0, NULL, &kernelEvent);
    if (status != CL_SUCCESS) {
		cout << "Error in clEnqueueNDRangeKernel. Error code = " << status  << endl;
	}

    // Obtain buffer
    status = clEnqueueReadBuffer(commandQueue, d_buffer, CL_TRUE, 0,  sizeof(char)*100000, buffer, 0, NULL, &readEvent);

    cout << "Program finished: " << endl;
    cout << "Result: " << buffer;
}

// ///////////////////////////////////////////////////////
// Private methods
// ///////////////////////////////////////////////////////
char* OCLVM::readSource(const char *sourceFilename) {
    FILE *fp;
    int err;
    int size;
    char *source;
    fp = fopen(sourceFilename, "rb");
    if(fp == NULL) {
       printf("Could not open kernel file: %s\n", sourceFilename);
       exit(-1);
    }
    err = fseek(fp, 0, SEEK_END);
    if(err != 0) {
       printf("Error seeking to end of file\n");
       exit(-1);
 
    }
    size = ftell(fp);
    if(size < 0) {
       printf("Error getting file position\n");
       exit(-1);
    }
    err = fseek(fp, 0, SEEK_SET);
    if(err != 0) {
       printf("Error seeking to start of file\n");
       exit(-1);
 
    }
    source = (char*)malloc(size+1);
    if(source == NULL) {
       printf("Error allocating %d bytes for the program source\n", size+1);
       exit(-1);
    }
    err = fread(source, 1, size, fp);
    if(err != size) {
       printf("only read %d bytes\n", err);
       exit(0);
    } 
    source[size] = '\0';
    return source;
}
