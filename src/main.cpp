#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
using namespace std;

#include "instruction.hpp"
#include "bytecodes.hpp"
#include "vm.hpp"
#include "oclVM.hpp"
#include "stats.hpp"

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
    OCLVM oclVM(vectorAdd, 0);
    oclVM.setVMConfig(100, 100);
    oclVM.setTrace();
    oclVM.setPlatform(0);
    oclVM.initHeap();
    oclVM.initOpenCL("src/interpreter.cl", false);
    oclVM.runInterpreter();
    oclVM.printHeap();
    cout << endl;
}

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
    oclVM.initOpenCL("src/interpreter.cl", false);
    oclVM.runInterpreter();
}

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
    oclVM.initOpenCL("src/interpreterLocal.cl", false);
    oclVM.runInterpreter();
}


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
    oclVM.initOpenCL("src/interpreterPrivate.cl", false);
    oclVM.runInterpreter();
}

void runBenchmarkCplus() {
    // Vector addition in a LOOP
    vector<int> vectorAdd = {
            ICONST, 0,
            DUP,
            ICONST, 50000000,    // Define the vector size
            IEQ,
            BRT, 23,
            DUP,    // offset for each array to load
            DUP,    // offset for each array to load
            GLOAD_INDEXED, 50000000,
            LOAD, 1,   // load from position 1
            GLOAD_INDEXED, 100000000,
            IADD,
            GSTORE_INDEXED, 0,
            ICONST1,
            IADD,
            BR, 2,
            POP,
            HALT
    };

    vector<double> totalTime;

    for (int i = 0; i < 11; i++) {
        VM vm(vectorAdd,  0); 
        vm.setVMConfig(100, 150000000);
        vm.initHeap();
        auto start_time = chrono::high_resolution_clock::now();
        vm.runInterpreter();
        auto end_time = chrono::high_resolution_clock::now();
        double totalSeq = chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count();
        totalTime.push_back(totalSeq);
    }

    double medianTotalTime = median(totalTime);
    cout << "Median TotalTime: " << medianTotalTime << endl;
}

void runBenchmarkOpenCL() {
    // Vector addition in a LOOP
    vector<int> vectorAdd = {
            ICONST, 0,
            DUP,
            ICONST, 50000000,    // Define the vector size
            IEQ,
            BRT, 23,
            DUP,    // offset for each array to load
            DUP,    // offset for each array to load
            GLOAD_INDEXED, 50000000,
            LOAD, 1,   // load from position 1
            GLOAD_INDEXED, 100000000,
            IADD,
            GSTORE_INDEXED, 0,
            ICONST1,
            IADD,
            BR, 2,
            POP,
            HALT
    };

    vector<long> totalTime;

    for (int i = 0; i < 11; i++) {
        OCLVM oclVM(vectorAdd, 0);
        oclVM.setVMConfig(100, 150000000);
        oclVM.setPlatform(0);
        oclVM.initOpenCL("src/interpreter.cl", false);
        oclVM.initHeap();
        oclVM.runInterpreter();
        long kernelTime = oclVM.getKernelTime();
        totalTime.push_back(kernelTime);
    }

    double medianTotalTime = median(totalTime);
    cout << "Median OpenCLTimer: " << medianTotalTime << endl;
}

void runOpenCLParallelIntepreter() {
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
    oclVM.initOpenCL("src/interpreterParallel.cl", false);
    oclVM.initHeap();
    oclVM.runInterpreter(1024);
}


void runOpenCLParallelIntepreterLoop() {
    int size = 1024;
    int groupSize = 256;
    vector<int> vectorAdd = {
        THREAD_ID,
        DUP,
        ICONST, groupSize,
        IEQ,
        BRT, 23,
        DUP,    // offset for each array to load
        DUP,    // offset for each array to load
        PARALLEL_GLOAD_INDEXED, 0,
        LOAD, 1,   // load index from position 1
        PARALLEL_GLOAD_INDEXED, 1,
        IMUL,
        PARALLEL_GSTORE_INDEXED, 2,
        ICONST1,
        IADD,
        BR, 1,
        POP,
        HALT
    };
    OCLVMParallelLoop oclVM(vectorAdd, 0);
    oclVM.setVMConfig(100, size);
    oclVM.setHeapSizes(size);
    oclVM.setPlatform(0);
    oclVM.initOpenCL("src/interpreterParallelLoop.cl", false);
    oclVM.initHeap();
    oclVM.runInterpreter(size, groupSize);
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

void runBenchmarks() {
    runBenchmarkCplus();
    runBenchmarkOpenCL();
}

int main(int argc, char** argv) {
    runOpenCLParallelIntepreter();
    runOpenCLParallelIntepreterLoop();
    return 0;
}