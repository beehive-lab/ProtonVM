/*
 * Copyright (c) 2020-2021, APT Group, Department of Computer Science,
 * The University of Manchester.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef OCLVM_HPP
#define OCLVM_HPP

#define CL_TARGET_OPENCL_VERSION 300

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
        OCLVM() {};
        OCLVM(vector<int> code, int mainByteCodeIndex);

        ~OCLVM();

        virtual int initOpenCL(string kernelFilename, bool loadBinary);

        void createBuffers();

        void setPlatform(int numPlatform);

        void useLocalMemory();

        void usePrivateMemory();

        long getKernelTime();

        // Implementation of the Interpreter in OpenCL C
        virtual void runInterpreter();

    protected:

        char* readSource(const char* sourceFilename);
        int readBinaryFile(unsigned char **output, size_t *size, const char *name);
        long getTime(cl_event event);

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

        cl_event writeEvent[5];
        cl_event kernelEvent;
        cl_event readEvent[5];

        int platformNumber = 0;

        char* buffer;

        bool useLocal = false;
        bool usePrivate = false;

        cl_mem d_code;
        cl_mem d_stack;
        cl_mem d_data;
        cl_mem d_buffer;

        bool buffersCreated = false;

        const int BUFFER_SIZE = 100000;
};

class OCLVMPrivate : public OCLVM {
    public:
        OCLVMPrivate() {};
        OCLVMPrivate(vector<int> code, int mainByteCodeIndex);
        void runInterpreter();

};

class OCLVMParallel : public OCLVM {
    public:
        OCLVMParallel() {};
        OCLVMParallel(vector<int> code, int mainByteCodeIndex);
        void runInterpreter(size_t range);
        void setHeapSizes(int dataSize);
        void initHeap();

    protected:
        vector<int> data1;
        vector<int> data2;
        vector<int> data3;
};

class OCLVMParallelLoop : public OCLVMParallel {
    public:
        OCLVMParallelLoop() {};
        OCLVMParallelLoop(vector<int> code, int mainByteCodeIndex);
        void runInterpreter(size_t globalWordItems, size_t localWorkItems);

};

#endif 