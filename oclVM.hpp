#ifndef OCLVM_HPP
#define OCLVM_HPP

#include <iostream>
#include <string>
#include "instruction.hpp"
#include "bytecodes.hpp"
#include "abstractVM.hpp"

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#ifdef __APPLE__
	#include <OpenCL/cl.h>
#else
	#include <CL/cl.h>
#endif

using namespace std;

class OCLVM : public AbstractVM {

    public:
        OCLVM(int* code, int mainByteCodeIndex);

        ~OCLVM();

        int initOpenCL();

        void setPlatform(int numPlatform);

        // Implementation of the Interpreter in 
        // OpenCL C
        void runInterpreter();

    private:

        char* readSource(const char* sourceFilename);

        string platformName;
        cl_uint numPlatforms;
        cl_platform_id *platforms;
        cl_device_id *devices;
        cl_context context;
        cl_command_queue commandQueue;
        cl_kernel kernel1;
        cl_kernel kernel2;
        cl_program program;
        char *source;
        size_t localWorkSize[1];
        size_t lWorkSize;

        int platformNumber = 0;

};

#endif 