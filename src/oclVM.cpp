#include <iostream>
#include <string>
#include <vector>
#include "instruction.hpp"
#include "bytecodes.hpp"
#include "oclVM.hpp"

#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define DEBUG 1

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

        // Release OpenCL objects
        // clReleaseKernel(kernel1);
        // clReleaseProgram(program);  
        // clReleaseCommandQueue(commandQueue);
        // clReleaseMemObject(d_buffer);
        // clReleaseMemObject(d_code);
        // clReleaseMemObject(d_stack);
        // clReleaseMemObject(d_data);
        // clReleaseContext(context);
        // free(source);
        // free(platforms);
        // free(devices);
    }
}

void OCLVM::setPlatform(int numPlatform) {
    this->platformNumber = numPlatform;
}

void OCLVM::useLocalMemory() {
    this->useLocal = true;
}

void OCLVM::usePrivateMemory() {
    this->usePrivate = true;
}

long OCLVM::getKernelTime() {
    if (kernelEvent != nullptr) {
        return OCLVM::getTime(kernelEvent);
    }
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
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
		devices = (cl_device_id*) malloc(numDevices*sizeof(cl_device_id));
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
	} else {
		devices = (cl_device_id*) malloc(numDevices*sizeof(cl_device_id));
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
	}
	
	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
	
	commandQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &status);	
	if (status != CL_SUCCESS || commandQueue == NULL) {
		cout << "Error in create command. Error code = " << status  << endl;
		return -1;
	}

    if (loadBinary) {
        unsigned char* binary = NULL;
        size_t program_size = 0;
        readBinaryFile(&binary, &program_size, kernelFilename.c_str());
 
        cl_device_id dev = devices[0];
        program = clCreateProgramWithBinary(context, 1, &dev, &program_size, (const unsigned char **) &binary,  NULL, &status);
        if (status != CL_SUCCESS) {
            cout << "Error in clCreateProgramWithBinary. Error code = " << status  << endl;
        }
        cl_int buildErr = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);
        free(binary);
        kernel1 = clCreateKernel(program, "interpreter", &status);
        if (status != CL_SUCCESS) {
            cout << "Error in clCreateKernel (interpreter). Code error: " << status << endl;
            abort();
        }
    } else { 
	    const char *sourceFile = kernelFilename.c_str();
	    source = readSource(sourceFile);
	    program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &status);
        if (status != CL_SUCCESS) {
            cout << "Error in clCreateProgramWithSource. Error code = " << status  << endl;
		    abort();	
        }

	    cl_int buildErr;
	    buildErr = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);
	    kernel1 = clCreateKernel(program, "interpreter", &status);
	    if (status != CL_SUCCESS) {
		    cout << "Error in clCreateKernel. Error code = " << status  << endl;
		    abort();	
	    }
    }
}

void OCLVM::createBuffers() {
    this->buffer = new char[BUFFER_SIZE];

    // Create all buffers
    cl_int status;
 	d_code = clCreateBuffer(context, CL_MEM_READ_ONLY, codeSize * sizeof(int), NULL, &status);
    if (status != CL_SUCCESS) {
        cout << "Error in clCreateBuffer: " << status << endl;
    }
    d_stack = clCreateBuffer(context, CL_MEM_READ_WRITE, stackSize * sizeof(int), NULL, &status);
    d_data = clCreateBuffer(context, CL_MEM_READ_WRITE, dataSize * sizeof(int), NULL, &status);
    d_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, BUFFER_SIZE * sizeof(char), NULL, &status);
}

void OCLVM::runInterpreter() {

    if (!buffersCreated) {
        createBuffers();
        buffersCreated = true;
    }

    // Copy code from HOST->DEVICE
    cl_int status = clEnqueueWriteBuffer(commandQueue, d_code, CL_TRUE, 0, codeSize * sizeof(int), code.data(), 0, NULL, &writeEvent[0]);
    status |= clEnqueueWriteBuffer(commandQueue, d_data, CL_TRUE, 0, dataSize * sizeof(int), data.data(), 0, NULL, &writeEvent[1]);
    if (status != CL_SUCCESS) {
        cout << "Error in clEnqueueWriteBuffer. Error code = " << status  << endl;
    }
    
    int t = (trace)? 1: 0;
    // Set arguments to the kernel
	status  = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &d_code);
    status |= clSetKernelArg(kernel1, 1, sizeof(cl_mem), &d_stack);
    status |= clSetKernelArg(kernel1, 2, sizeof(cl_mem), &d_data);
    status |= clSetKernelArg(kernel1, 3, sizeof(cl_mem), &d_buffer);
	status |= clSetKernelArg(kernel1, 4, sizeof(cl_int), &codeSize);
    status |= clSetKernelArg(kernel1, 5, sizeof(cl_int), &ip);
    status |= clSetKernelArg(kernel1, 6, sizeof(cl_int), &fp);
    status |= clSetKernelArg(kernel1, 7, sizeof(cl_int), &sp);
    status |= clSetKernelArg(kernel1, 8, sizeof(cl_int), &t);
    if (status != CL_SUCCESS) {
		cout << "Error in clSetKernelArgs. Error code = " << status  << endl;
	}

    // Launch Kernel with 1 thread local and global
    size_t globalWorkSize[] = {1};
    size_t localWorkSize[] = {1};
    status = clEnqueueNDRangeKernel(commandQueue, kernel1, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, &kernelEvent);
    if (status != CL_SUCCESS) {
		cout << "Error in clEnqueueNDRangeKernel. Error code = " << status  << endl;
	}

    // Obtain buffer and heap (DEVICE -> HOST)
    status = clEnqueueReadBuffer(commandQueue, d_buffer, CL_TRUE, 0,  sizeof(char) * BUFFER_SIZE, buffer, 0, NULL, &readEvent[0]);
    status |= clEnqueueReadBuffer(commandQueue, d_data, CL_TRUE, 0,  sizeof(int) * data.size(), data.data(), 0, NULL, &readEvent[1]);
    if (status != CL_SUCCESS) {
        cout << "Error in clEnqueueReadBuffer. Error code = " << status  << endl;
    }
    cout << "Program finished: " << endl;
    if (DEBUG) {
        for (auto i = 0; i < data.size(); i++) {
            cout << data[i]  << " ";
        }
        cout << "\n";
    }
}

// ====================================================================
// OCLVMLocal Class
// ====================================================================
OCLVMLocal::OCLVMLocal(vector<int> code, int mainByteCodeIndex) {
    this->code = code;
    this->codeSize = code.size();
    this->ip = mainByteCodeIndex;
    this->ins = createAllInstructions();
}

void OCLVMLocal::runInterpreter() {

    cout << "Running LOCAL" << endl;

    this->buffer = new char[BUFFER_SIZE];

    // Create all buffers
    cl_int status;
 	cl_mem d_code = clCreateBuffer(context, CL_MEM_READ_ONLY, codeSize * sizeof(int), NULL, &status);
    if (status != CL_SUCCESS) {
        cout << "Error in clCreateBuffer: " << status << endl;
    }
    cl_mem d_stack = clCreateBuffer(context, CL_MEM_READ_WRITE, stackSize * sizeof(int), NULL, &status);
    cl_mem d_data = clCreateBuffer(context, CL_MEM_READ_WRITE, dataSize * sizeof(int), NULL, &status);
    cl_mem d_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, BUFFER_SIZE * sizeof(char), NULL, &status);
    
    // Copy code from HOST->DEVICE
    status = clEnqueueWriteBuffer(commandQueue, d_code, CL_TRUE, 0, codeSize * sizeof(int), code.data(), 0, NULL, &writeEvent[0]);
    status |= clEnqueueWriteBuffer(commandQueue, d_data, CL_TRUE, 0, dataSize * sizeof(int), data.data(), 0, NULL, &writeEvent[1]);
    if (status != CL_SUCCESS) {
        cout << "Error in clEnqueueWriteBuffer. Error code = " << status  << endl;
    }
    
    int t = (trace)? 1: 0;
    // Push Arguments
	status  = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &d_code);
    status |= clSetKernelArg(kernel1, 1, stackSize * sizeof(cl_int), NULL);
    status |= clSetKernelArg(kernel1, 2, sizeof(cl_mem), &d_data);
    status |= clSetKernelArg(kernel1, 3, sizeof(cl_mem), &d_buffer);
	status |= clSetKernelArg(kernel1, 4, sizeof(cl_int), &codeSize);
    status |= clSetKernelArg(kernel1, 5, sizeof(cl_int), &ip);
    status |= clSetKernelArg(kernel1, 6, sizeof(cl_int), &fp);
    status |= clSetKernelArg(kernel1, 7, sizeof(cl_int), &sp);
    status |= clSetKernelArg(kernel1, 8, sizeof(cl_int), &t);
    if (status != CL_SUCCESS) {
		cout << "Error in clSetKernelArgs. Error code = " << status  << endl;
	}

    // Launch Kernel
    size_t globalWorkSize[] = {1};
    size_t localWorkSize[] = {1};
    status = clEnqueueNDRangeKernel(commandQueue, kernel1, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, &kernelEvent);
    if (status != CL_SUCCESS) {
		cout << "Error in clEnqueueNDRangeKernel. Error code = " << status  << endl;
	}

    // Obtain buffer and heap
    status = clEnqueueReadBuffer(commandQueue, d_buffer, CL_TRUE, 0,  sizeof(char) * BUFFER_SIZE, buffer, 0, NULL, &readEvent[0]);
    status |= clEnqueueReadBuffer(commandQueue, d_data, CL_TRUE, 0,  sizeof(int) * data.size(), data.data(), 0, NULL, &readEvent[1]);
     if (status != CL_SUCCESS) {
        cout << "Error in clEnqueueReadBuffer. Error code = " << status  << endl;
    }

    cout << "Program finished: " << endl;
    cout << "Result: " << buffer;
}

// ====================================================================
// OCLVMPrivate Class
// ====================================================================
OCLVMPrivate::OCLVMPrivate(vector<int> code, int mainByteCodeIndex) {
    this->code = code;
    this->codeSize = code.size();
    this->ip = mainByteCodeIndex;
    this->ins = createAllInstructions();
}

void OCLVMPrivate::runInterpreter() {

    cout << "Running PRIVATE" << endl;

    this->buffer = new char[BUFFER_SIZE];

    // Create all buffers
    cl_int status;
 	cl_mem d_code = clCreateBuffer(context, CL_MEM_READ_ONLY, codeSize * sizeof(int), NULL, &status);
    if (status != CL_SUCCESS) {
        cout << "Error in clCreateBuffer: " << status << endl;
    }
    cl_mem d_stack = clCreateBuffer(context, CL_MEM_READ_WRITE, stackSize * sizeof(int), NULL, &status);
    cl_mem d_data = clCreateBuffer(context, CL_MEM_READ_WRITE, dataSize * sizeof(int), NULL, &status);
    cl_mem d_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, BUFFER_SIZE * sizeof(char), NULL, &status);
    
    // Copy code from HOST->DEVICE
    status = clEnqueueWriteBuffer(commandQueue, d_code, CL_TRUE, 0, codeSize * sizeof(int), code.data(), 0, NULL, &writeEvent[0]);
    status |= clEnqueueWriteBuffer(commandQueue, d_data, CL_TRUE, 0, dataSize * sizeof(int), data.data(), 0, NULL, &writeEvent[1]);
    if (status != CL_SUCCESS) {
        cout << "Error in clEnqueueWriteBuffer. Error code = " << status  << endl;
    }
    
    int t = (trace)? 1: 0;
    // Push Arguments
	status  = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &d_code);
    status |= clSetKernelArg(kernel1, 1, sizeof(cl_mem), &d_data);
    status |= clSetKernelArg(kernel1, 2, sizeof(cl_mem), &d_buffer);
	status |= clSetKernelArg(kernel1, 3, sizeof(cl_int), &codeSize);
    status |= clSetKernelArg(kernel1, 4, sizeof(cl_int), &ip);
    status |= clSetKernelArg(kernel1, 5, sizeof(cl_int), &fp);
    status |= clSetKernelArg(kernel1, 6, sizeof(cl_int), &sp);
    status |= clSetKernelArg(kernel1, 7, sizeof(cl_int), &t);
    if (status != CL_SUCCESS) {
		cout << "Error in clSetKernelArgs. Error code = " << status  << endl;
	}

    // Launch Kernel
    size_t globalWorkSize[] = {1};
    size_t localWorkSize[] = {1};
    status = clEnqueueNDRangeKernel(commandQueue, kernel1, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, &kernelEvent);
    if (status != CL_SUCCESS) {
		cout << "Error in clEnqueueNDRangeKernel. Error code = " << status  << endl;
	}

    // Obtain buffer and heap
    status = clEnqueueReadBuffer(commandQueue, d_buffer, CL_TRUE, 0,  sizeof(char) * BUFFER_SIZE, buffer, 0, NULL, &readEvent[0]);
    status |= clEnqueueReadBuffer(commandQueue, d_data, CL_TRUE, 0,  sizeof(int) * data.size(), data.data(), 0, NULL, &readEvent[1]);
     if (status != CL_SUCCESS) {
        cout << "Error in clEnqueueReadBuffer. Error code = " << status  << endl;
    }

    cout << "Program finished: " << endl;
    cout << "Result: " << buffer;
}

// ====================================================================
// OCLVMParallel Class
// ====================================================================
OCLVMParallel::OCLVMParallel(vector<int> code, int mainByteCodeIndex) {
    this->code = code;
    this->codeSize = code.size();
    this->ip = mainByteCodeIndex;
    this->ins = createAllInstructions();
}

void OCLVMParallel::setHeapSizes(int dataSize) {
    this->data1.resize(dataSize);
    this->data2.resize(dataSize);
    this->data3.resize(dataSize);
}

void OCLVMParallel::initHeap() {
    for (auto i = 0; i < data1.size(); i++) {
        data1[i] = i;
    }
    for (auto i = 0; i < data2.size(); i++) {
        data2[i] = i;
    }
    for (auto i = 0; i < data3.size(); i++) {
        data3[i] = 1;
    }
}

void OCLVMParallel::runInterpreter(size_t range) {

    this->buffer = new char[BUFFER_SIZE];

    // Create all buffers
    cl_int status;
 	cl_mem d_code = clCreateBuffer(context, CL_MEM_READ_ONLY, codeSize * sizeof(int), NULL, &status);
    cl_mem d_data1 = clCreateBuffer(context, CL_MEM_READ_WRITE, data1.size() * sizeof(int), NULL, &status);
    cl_mem d_data2 = clCreateBuffer(context, CL_MEM_READ_WRITE, data2.size() * sizeof(int), NULL, &status);
    cl_mem d_data3 = clCreateBuffer(context, CL_MEM_READ_WRITE, data3.size() * sizeof(int), NULL, &status);
    cl_mem d_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, BUFFER_SIZE * sizeof(char), NULL, &status);
    
    // Copy code from HOST->DEVICE
    status = clEnqueueWriteBuffer(commandQueue, d_code, CL_TRUE, 0, codeSize * sizeof(int), code.data(), 0, NULL, &writeEvent[0]);
    status |= clEnqueueWriteBuffer(commandQueue, d_data1, CL_TRUE, 0, dataSize * sizeof(int), data1.data(), 0, NULL, &writeEvent[1]);
    status |= clEnqueueWriteBuffer(commandQueue, d_data2, CL_TRUE, 0, dataSize * sizeof(int), data2.data(), 0, NULL, &writeEvent[2]);
    status |= clEnqueueWriteBuffer(commandQueue, d_data3, CL_TRUE, 0, dataSize * sizeof(int), data3.data(), 0, NULL, &writeEvent[3]);
    if (status != CL_SUCCESS) {
        cout << "Error in clEnqueueWriteBuffer. Error code = " << status  << endl;
    }
    
    int t = (trace)? 1: 0;
    // Push Arguments
	status  = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &d_code);
    status |= clSetKernelArg(kernel1, 1, sizeof(cl_mem), &d_data1);
    status |= clSetKernelArg(kernel1, 2, sizeof(cl_mem), &d_data2);
    status |= clSetKernelArg(kernel1, 3, sizeof(cl_mem), &d_data3);
    status |= clSetKernelArg(kernel1, 4, sizeof(cl_mem), &d_buffer);
	status |= clSetKernelArg(kernel1, 5, sizeof(cl_int), &codeSize);
    status |= clSetKernelArg(kernel1, 6, sizeof(cl_int), &ip);
    status |= clSetKernelArg(kernel1, 7, sizeof(cl_int), &fp);
    status |= clSetKernelArg(kernel1, 8, sizeof(cl_int), &sp);
    status |= clSetKernelArg(kernel1, 9, sizeof(cl_int), &t);
    if (status != CL_SUCCESS) {
		cout << "Error in clSetKernelArgs. Error code = " << status  << endl;
	}

    // Launch Kernel
    size_t globalWorkSize[] = {range};
    size_t localWorkSize[] = {1};
    status = clEnqueueNDRangeKernel(commandQueue, kernel1, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, &kernelEvent);
    if (status != CL_SUCCESS) {
		cout << "Error in clEnqueueNDRangeKernel. Error code = " << status  << endl;
	}

    // Obtain buffer and heap
    status = clEnqueueReadBuffer(commandQueue, d_buffer, CL_TRUE, 0,  sizeof(char) * BUFFER_SIZE, buffer, 0, NULL, &readEvent[0]);
    status |= clEnqueueReadBuffer(commandQueue, d_data1, CL_TRUE, 0,  sizeof(int) * data1.size(), data1.data(), 0, NULL, &readEvent[1]);
    status |= clEnqueueReadBuffer(commandQueue, d_data2, CL_TRUE, 0,  sizeof(int) * data2.size(), data2.data(), 0, NULL, &readEvent[2]);
    status |= clEnqueueReadBuffer(commandQueue, d_data3, CL_TRUE, 0,  sizeof(int) * data3.size(), data3.data(), 0, NULL, &readEvent[3]);
     if (status != CL_SUCCESS) {
        cout << "Error in clEnqueueReadBuffer. Error code = " << status  << endl;
    }
}

// ====================================================================
// OCLVMParallelLoop Class
// ====================================================================
OCLVMParallelLoop::OCLVMParallelLoop(vector<int> code, int mainByteCodeIndex) {
    this->code = code;
    this->codeSize = code.size();
    this->ip = mainByteCodeIndex;
    this->ins = createAllInstructions();
}

void OCLVMParallelLoop::runInterpreter(size_t range1, size_t range2) {

    this->buffer = new char[BUFFER_SIZE];

    // Create all buffers
    cl_int status;
 	cl_mem d_code = clCreateBuffer(context, CL_MEM_READ_ONLY, codeSize * sizeof(int), NULL, &status);
    cl_mem d_data1 = clCreateBuffer(context, CL_MEM_READ_WRITE, data1.size() * sizeof(int), NULL, &status);
    cl_mem d_data2 = clCreateBuffer(context, CL_MEM_READ_WRITE, data2.size() * sizeof(int), NULL, &status);
    cl_mem d_data3 = clCreateBuffer(context, CL_MEM_READ_WRITE, data3.size() * sizeof(int), NULL, &status);
    cl_mem d_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, BUFFER_SIZE * sizeof(char), NULL, &status);
    
    // Copy code from HOST->DEVICE
    status = clEnqueueWriteBuffer(commandQueue, d_code, CL_TRUE, 0, codeSize * sizeof(int), code.data(), 0, NULL, &writeEvent[0]);
    status |= clEnqueueWriteBuffer(commandQueue, d_data1, CL_TRUE, 0, dataSize * sizeof(int), data1.data(), 0, NULL, &writeEvent[1]);
    status |= clEnqueueWriteBuffer(commandQueue, d_data2, CL_TRUE, 0, dataSize * sizeof(int), data2.data(), 0, NULL, &writeEvent[2]);
    status |= clEnqueueWriteBuffer(commandQueue, d_data3, CL_TRUE, 0, dataSize * sizeof(int), data3.data(), 0, NULL, &writeEvent[3]);
    if (status != CL_SUCCESS) {
        cout << "Error in clEnqueueWriteBuffer. Error code = " << status  << endl;
    }
    
    int t = (trace)? 1: 0;
    // Push Arguments
	status  = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &d_code);
    status |= clSetKernelArg(kernel1, 1, sizeof(cl_mem), &d_data1);
    status |= clSetKernelArg(kernel1, 2, sizeof(cl_mem), &d_data2);
    status |= clSetKernelArg(kernel1, 3, sizeof(cl_mem), &d_data3);
    status |= clSetKernelArg(kernel1, 4, sizeof(cl_mem), &d_buffer);
	status |= clSetKernelArg(kernel1, 5, sizeof(cl_int), &codeSize);
    status |= clSetKernelArg(kernel1, 6, sizeof(cl_int), &ip);
    status |= clSetKernelArg(kernel1, 7, sizeof(cl_int), &fp);
    status |= clSetKernelArg(kernel1, 8, sizeof(cl_int), &sp);
    status |= clSetKernelArg(kernel1, 9, sizeof(cl_int), &t);
    if (status != CL_SUCCESS) {
		cout << "Error in clSetKernelArgs. Error code = " << status  << endl;
	}

    // Launch Kernel
    size_t globalWorkSize[] = {range1};
    size_t localWorkSize[] = {range2};
    status = clEnqueueNDRangeKernel(commandQueue, kernel1, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, &kernelEvent);
    if (status != CL_SUCCESS) {
		cout << "Error in clEnqueueNDRangeKernel. Error code = " << status  << endl;
	}

    // Obtain buffer and heap
    status = clEnqueueReadBuffer(commandQueue, d_buffer, CL_TRUE, 0,  sizeof(char) * BUFFER_SIZE, buffer, 0, NULL, &readEvent[0]);
    status |= clEnqueueReadBuffer(commandQueue, d_data1, CL_TRUE, 0,  sizeof(int) * data1.size(), data1.data(), 0, NULL, &readEvent[1]);
    status |= clEnqueueReadBuffer(commandQueue, d_data2, CL_TRUE, 0,  sizeof(int) * data2.size(), data2.data(), 0, NULL, &readEvent[2]);
    status |= clEnqueueReadBuffer(commandQueue, d_data3, CL_TRUE, 0,  sizeof(int) * data3.size(), data3.data(), 0, NULL, &readEvent[3]);
    if (status != CL_SUCCESS) {
        cout << "Error in clEnqueueReadBuffer. Error code = " << status  << endl;
    }

    if (DEBUG) {
        for (auto i = 0; i < data3.size(); i++) {
            cout << data3[i]  << " ";
        }
        cout << "\n";
    }
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

long OCLVM::getTime(cl_event event) {
    clWaitForEvents(1, &event);
    cl_ulong time_start, time_end;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
    return (time_end - time_start);
}