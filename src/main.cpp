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

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

#include "bytecodes.hpp"
#include "vm.hpp"
#include "oclVM.hpp"

/// ***************************************************************************************************************************
/// Run the hello world program.
/// This function computes an addition between two numbers of top of the stack and stores the result
/// in main memory.
/// This functions runs the sequential BC interpreter implemented in C++.
/// ***************************************************************************************************************************
void testHello() {
    vector<int> hello = {
        ICONST, 128,
        ICONST, 1,
        IADD,
        GSTORE, 0,
        GLOAD, 0,
        PRINT,
        HALT
    };
    VM vm(hello,  0);
    vm.setVMConfig(100, 100);
    vm.setTrace();
    vm.runInterpreter();
}

/// ***************************************************************************************************************************
/// Test for running the sequential C++ BC interpreter on CPU.
/// ***************************************************************************************************************************
void testProgram1() {
    vector<int> program1 = {
        ICONST, 100,
        ICONST, 100,
        IADD,
        GSTORE, 2,
        GLOAD, 2,
        PRINT,
        HALT
    };
    VM vm(program1,  0);
    vm.setVMConfig(100, 100);
    vm.setTrace();
    vm.runInterpreter();
}

/// ***************************************************************************************************************************
/// Test for running the sequential C++ BC interpreter on CPU.
/// ***************************************************************************************************************************
void testProgram2() {
    vector<int> program2 = {
        ICONST, 100,
        LSHIFT,
        DUP,
        PRINT,
        RSHIFT,
        PRINT,
        HALT
    };
    VM vm(program2,  0);
    vm.setVMConfig(100, 100);
    vm.setTrace();
    vm.runInterpreter();
}

/// ***************************************************************************************************************************
/// Test function calls for the sequential C++ BC interpreter on CPU.
/// ***************************************************************************************************************************
void testFunction() {
    vector<int> functionCall = {
        // Instruction    address
        LOAD, -3,		// 0
        ICONST, 2,		// 2
        IMUL,			// 4
        RET,			// 5

        ICONST, 128, 	// 6 -- this is the main
        CALL, 0, 1, 	// 8
        PRINT,			// 11
        HALT			// 12
    };
    VM vm(functionCall,  6); // Start address is 6
    vm.setVMConfig(100, 100);
    vm.setTrace();
    vm.runInterpreter();
}

/// ***************************************************************************************************************************
/// Test the vector addition on CPU using the sequential C++ BC interpreter.
/// ***************************************************************************************************************************
void testVectorAddition() {
    // Vector addition in a LOOP
    vector<int> vectorAdd = {
            ICONST, 0,
            DUP,
            ICONST, 10,
            IEQ,
            BRT, 23,
            DUP,    // offset for each array to load
            DUP,    // offset for each array to load
            GLOAD_INDEXED, 10,
            LOAD, 1,   // load from position 1
            GLOAD_INDEXED, 20,
            IADD,
            GSTORE_INDEXED, 0,
            ICONST1,
            IADD,
            BR, 2,
            POP,
            HALT
    };
    VM vm(vectorAdd,  0); 
    vm.setVMConfig(100, 100);
    vm.setTrace();
    vm.initHeap();
    vm.runInterpreter();
    vm.printHeap();
    cout << endl;
}

/// ***************************************************************************************************************************
/// Test for running vector addition with OpenCL on any heterogeneous device (e.g., a GPU).
/// OCLVM runs a sequential BC interpreter on the GPU. This means that ProtoVM launches a single
/// thread kernel that runs the whole application on the device.
/// ***************************************************************************************************************************
void testVectorAdditionOpenCL() {
    // Vector addition in a LOOP
    vector<int> vectorAdd = {
            ICONST, 0,
            DUP,
            ICONST, 10,
            IEQ,
            BRT, 23,
            DUP,    // offset for each array to load
            DUP,    // offset for each array to load
            GLOAD_INDEXED, 10,
            LOAD, 1,   // load from position 1
            GLOAD_INDEXED, 20,
            IADD,
            GSTORE_INDEXED, 0,
            ICONST1,
            IADD,
            BR, 2,
            POP,
            HALT
    };
    OCLVM oclVM(vectorAdd, 0);    /// OpenCL BC interpreter (single-thread).
    oclVM.setVMConfig(100, 100);
    oclVM.setTrace();
    oclVM.setPlatform(0);
    oclVM.initHeap();
    oclVM.initOpenCL("lib/interpreter.cl", false);
    oclVM.runInterpreter();
    oclVM.printHeap();
    cout << endl;
}

/// ***************************************************************************************************************************
/// Test for running simple addition using the single-threaded OpenCL BC interpreter on any heterogeneous device (e.g., a GPU).
/// OCLVM runs a sequential BC interpreter on the GPU. This means that ProtoVM launches a single
/// thread kernel that runs the whole application on the device.
/// ***************************************************************************************************************************
void testOpenCLInterpreter() {
    vector<int> hello = {
        ICONST, 128,
        ICONST, 1,
        IADD,
        GSTORE, 0,
        GLOAD, 0,
        PRINT,
        HALT
    };
    // OpenCL Interpreter
    OCLVM oclVM(hello, 0);
    oclVM.setVMConfig(100, 100);
    oclVM.setPlatform(0);
    oclVM.setTrace();
    oclVM.initOpenCL("lib/interpreter.cl", false);
    oclVM.runInterpreter();
}

/// ***************************************************************************************************************************
/// Test for running simple addition using the single-threaded OpenCL BC interpreter on any heterogeneous device (e.g., a GPU).
/// OCLVMLocal runs a sequential BC interpreter on the GPU. This means that ProtoVM launches a single
/// thread kernel that runs the whole application on the device. The stack is stored in local (shared) memory. This version was designed
/// only for testing on FPGAs and analyze the resource utilization.
/// ***************************************************************************************************************************
void testOpenCLInterpreterLocal() {
    vector<int> hello = {
        ICONST, 128,
        ICONST, 1,
        IADD,
        GSTORE, 0,
        GLOAD, 0,
        PRINT,
        HALT
    };
    // OpenCL Interpreter
    OCLVMLocal oclVM(hello, 0);
    oclVM.setVMConfig(100, 100);
    oclVM.setPlatform(0);
    oclVM.setTrace();
    oclVM.initOpenCL("lib/interpreterLocal.cl", false);
    oclVM.runInterpreter();
}

/// ***************************************************************************************************************************
/// Test for running simple addition using the single-threaded OpenCL BC interpreter on any heterogeneous device (e.g., a GPU).
/// OCLVMPrivate runs a sequential BC interpreter on the GPU. This means that ProtoVM launches a single
/// thread kernel that runs the whole application on the device. The stack is stored in private memory.
/// ***************************************************************************************************************************
void testOpenCLInterpreterPrivate() {
    vector<int> hello = {
        ICONST, 128,
        ICONST, 1,
        IADD,
        GSTORE, 0,
        GLOAD, 0,
        PRINT,
        HALT
    };
    // OpenCL Interpreter
    OCLVMPrivate oclVM(hello, 0);
    oclVM.setVMConfig(100, 100);
    oclVM.setPlatform(0);
    oclVM.setTrace();
    oclVM.initOpenCL("lib/interpreterPrivate.cl", false);
    oclVM.runInterpreter();
}

/// ***************************************************************************************************************************
/// Parallel BC Interpreter
/// ***************************************************************************************************************************
/// Test for running simple multiplication using the **PARALLEL** OpenCL BC interpreter on any heterogeneous device (e.g., a GPU).
/// OCLVMParallel launches many threads (as many as data items to compute). The stack is stored in local memory. Additionally,
/// this version uses a multi-heap approach and it makes uses of the BC THREAD_ID, which loads the thread-id onto the stack,
/// and PARALLEL_GLOAD_INDEXED,PARALLEL_GSTORE_INDEXED for accessing data using the thread-id in a multi-heap configuration.
///
/// This version was designed as an initial version for testing on FPGAs. Use the OCLVMParallelLoop instead.
/// ***************************************************************************************************************************
void runOpenCLParallelInterpreter() {
    vector<int> vectorAdd = {
            THREAD_ID,
            DUP,
            PARALLEL_GLOAD_INDEXED, 0,          
            THREAD_ID,  
            PARALLEL_GLOAD_INDEXED, 1,        
            IMUL,
            PARALLEL_GSTORE_INDEXED, 2,
            HALT
    };
    OCLVMParallel oclVM(vectorAdd, 0);
    oclVM.setVMConfig(100, 1024);
    oclVM.setHeapSizes(1024);
    oclVM.setPlatform(0);
    oclVM.initOpenCL("lib/interpreterParallel.cl", false);
    oclVM.initHeap();
    oclVM.runInterpreter(1024);
}

/// ***************************************************************************************************************************
/// Parallel BC Interpreter: v2
/// ***************************************************************************************************************************1
/// Test for running simple multiplication using the **PARALLEL** OpenCL BC interpreter on any heterogeneous device (e.g., a GPU).
/// OCLVMParallel launches many threads (as many as data items to compute). The stack is stored in private memory. Additionally,
/// this version uses a multi-heap approach and it makes uses of the BC THREAD_ID, which loads the thread-id onto the stack,
/// and PARALLEL_GLOAD_INDEXED,PARALLEL_GSTORE_INDEXED for accessing data using the thread-id in a multi-heap configuration.
/// ***************************************************************************************************************************
/**
 * Note: the stack has to either in global memory
 *       or private memory. Otherwise results 
*        are wrong.
 */
void runOpenCLParallelIntepreterLoop() {
    int size = 1024;
    int groupSize = 16;
    vector<int> vectorMul = {
        THREAD_ID,
        DUP,
        PARALLEL_GLOAD_INDEXED, 0,          
        THREAD_ID,  
        PARALLEL_GLOAD_INDEXED, 1,        
        IMUL,
        PARALLEL_GSTORE_INDEXED, 2,
        HALT
    };
    OCLVMParallelLoop oclVM(vectorMul, 0);
    oclVM.setVMConfig(100, 1024);
    oclVM.setHeapSizes(1024);
    oclVM.setPlatform(0);
    oclVM.initOpenCL("lib/interpreterParallelLoop.cl", false);
    oclVM.initHeap();
    oclVM.runInterpreter(1024, groupSize);
}

void runTests() {
    std::cout << "----" << endl;
    testHello();
    std::cout << "----" << endl;
    testProgram1();
    std::cout << "----" << endl;
    testProgram2();
    std::cout << "----" << endl;
    testFunction();
    std::cout << "----" << endl;
    testVectorAddition();    

    // OpenCL Interpreter
    testOpenCLInterpreter();
}

int main(int argc, char** argv) {
    //testHello();
    //testOpenCLInterpreter();
    //runOpenCLParallelInterpreter();
    runOpenCLParallelIntepreterLoop();
    return 0;
}