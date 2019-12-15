#include <iostream>
#include <string>
#include "instruction.hpp"
#include "bytecodes.hpp"
#include "oclVM.hpp"

#include <stdio.h>
#include <stdlib.h>

using namespace std;

OCLVM::OCLVM(int* code, int mainByteCodeIndex) {
    this->code = code;
    this->codeSize = *(&code + 1) - code;
    this->ip = mainByteCodeIndex;
    this->ins = createAllInstructions();
}

OCLVM::~OCLVM() {
    if (vmAllocated) {
        delete[] this->stack;
        delete[] this->data;
    }
}

void OCLVM::setPlatform(int numPlatform) {
    this->platformNumber = numPlatform;
}

int OCLVM::initOpenCL() {
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

	const char *sourceFile = "interpreter.cl";
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

void OCLVM::runInterpreter() {

    // Create all buffers

    // Push Arguments

    // Launch Kernel

    // Obtain buffer

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
