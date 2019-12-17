#ifndef OCLVM_HPP
#define OCLVM_HPP

#include <iostream>
#include <string>
#include <vector>
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
        OCLVM(vector<int> code, int mainByteCodeIndex);

        ~OCLVM();

        int initOpenCL(string kernelFilename, bool loadBinary);

        void setPlatform(int numPlatform);

        // Implementation of the Interpreter in 
        // OpenCL C
        void runInterpreter();

    private:

        char* readSource(const char* sourceFilename);
        int readBinaryFile(unsigned char **output, size_t *size, const char *name);

        string platformName;
        cl_uint numPlatforms;
        cl_platform_id *platforms;
        cl_device_id *devices;
        cl_context context;
        cl_command_queue commandQueue;
        cl_kernel kernel1;
        cl_program program;
        char *source;
        size_t localWorkSize[1];
        size_t lWorkSize;

        cl_event writeEvent;
        cl_event writeEvent2;
        cl_event kernelEvent;
        cl_event readEvent;
        cl_event readEvent2;

        int platformNumber = 0;

        char* buffer;
};

#endif 