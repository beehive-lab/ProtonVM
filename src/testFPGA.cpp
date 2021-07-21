#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
using namespace std;

#include "bytecodes.hpp"
#include "vm.hpp"
#include "oclVM.hpp"
#include "stats.hpp"

int SIZE = 1024;

void runBenchmarkCplus() {
    // Vector multiplication in a LOOP
    vector<int> vectorAdd = {
            ICONST, 0,
            DUP,
            ICONST, SIZE,    // Define the vector size
            IEQ,
            BRT, 23,
            DUP,    // offset for each array to load
            DUP,    // offset for each array to load
            GLOAD_INDEXED, SIZE,
            LOAD, 1,   // load from position 1
            GLOAD_INDEXED, SIZE * 2,
            IMUL,
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
        vm.setVMConfig(100, SIZE * 3);
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

void runBenchmarkOpenCLSingleThread() {
    // Vector multiplication in a LOOP
    vector<int> vectorMul = {
            ICONST, 0,
            DUP,
            ICONST, SIZE,    // Define the vector size
            IEQ,
            BRT, 23,
            DUP,    // offset for each array to load
            DUP,    // offset for each array to load
            GLOAD_INDEXED, SIZE,
            LOAD, 1,   // load from position 1
            GLOAD_INDEXED, SIZE * 2,
            IMUL,
            GSTORE_INDEXED, 0,
            ICONST1,
            IADD,
            BR, 2,
            POP,
            HALT
    };

    vector<long> totalTime;

    // Run OpenCL Interpreter Single Thread using Global Memory
    OCLVM oclVM(vectorMul, 0);
    oclVM.setVMConfig(100, SIZE * 3);
    oclVM.setPlatform(1);
    oclVM.initOpenCL("lib/build/mykerinterpreternel.xclbin", true);
    for (int i = 0; i < 11; i++) {    
        oclVM.initHeap();
        oclVM.runInterpreter();
        long kernelTime = oclVM.getKernelTime();
        totalTime.push_back(kernelTime);
    }

    double medianTotalTime = median(totalTime);
    cout << "MedianGlobal OpenCLTimer: " << medianTotalTime << endl;

    totalTime.clear();

    // Run OpenCL Interpreter Single Thread using Local Memory
    OCLVMLocal oclVMLocal(vectorMul, 0);
    oclVMLocal.setVMConfig(100, SIZE * 3);
    oclVMLocal.setPlatform(1);
    oclVMLocal.initOpenCL("lib/buildLocal/mykerinterpreternelLocal.xclbin", true);
    for (int i = 0; i < 11; i++) {    
        oclVMLocal.initHeap();
        oclVMLocal.runInterpreter();
        long kernelTime = oclVMLocal.getKernelTime();
        totalTime.push_back(kernelTime);
    }

    medianTotalTime = median(totalTime);
    cout << "MedianLocal OpenCLTimer: " << medianTotalTime << endl;

    totalTime.clear();

    // Run OpenCL Interpreter Single Thread using Private Memory
    OCLVMPrivate oclVMPrivate(vectorMul, 0);
    oclVMPrivate.setVMConfig(100, SIZE * 3);
    oclVMPrivate.setPlatform(1);
    oclVMPrivate.initOpenCL("lib/buildLocal/mykerinterpreternelLocal.xclbin", true);
    for (int i = 0; i < 11; i++) {    
        oclVMPrivate.initHeap();
        oclVMPrivate.runInterpreter();
        long kernelTime = oclVMPrivate.getKernelTime();
        totalTime.push_back(kernelTime);
    }
    medianTotalTime = median(totalTime);
    cout << "MedianPrivate OpenCLTimer: " << medianTotalTime << endl;
}

void runOpenCLParallelIntepreter() {
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
    
    vector<long> totalTime;
    OCLVMParallel oclVM(vectorMul, 0);
    oclVM.setVMConfig(100, SIZE);
    oclVM.setHeapSizes(SIZE);
    oclVM.setPlatform(1);
    oclVM.initOpenCL("lib/buildParallel/mykerinterpreternelParallel.xclbin", true);
    for (int i = 0; i < 11; i++) {    
        oclVM.initHeap();
        oclVM.runInterpreter(SIZE);
        long kernelTime = oclVM.getKernelTime();
        totalTime.push_back(kernelTime);
    }
    double medianTotalTime = median(totalTime);
    cout << "MedianParallel OpenCLTimer: " << medianTotalTime << endl;
}

void runOpenCLParallelIntepreterLoop() {
    int size = SIZE;
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
    vector<long> totalTime;
    OCLVMParallelLoop oclVM(vectorMul, 0);
    oclVM.setVMConfig(100, SIZE);
    oclVM.setHeapSizes(SIZE);
    oclVM.setPlatform(1);
    oclVM.initOpenCL("lib/buildParallelLoop/mykerinterpreternelParallelLoop.xclbin", true);
    for (int i = 0; i < 11; i++) {    
        oclVM.initHeap();
        oclVM.runInterpreter(SIZE, groupSize);
        long kernelTime = oclVM.getKernelTime();
        totalTime.push_back(kernelTime);
    }
    double medianTotalTime = median(totalTime);
    cout << "MedianParallel OpenCLTimer: " << medianTotalTime << endl;
}

void runBenchmarks() {
    runBenchmarkCplus();
    runOpenCLParallelIntepreterLoop();
}

void runHello() {
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
    oclVM.setTrace();
    oclVM.setPlatform(1);
    oclVM.initOpenCL("interpreter.aocx", true);
    oclVM.runInterpreter();

}

int main(int argc, char** argv) {
    if (argc > 1) {
        SIZE = atoi(argv[1]);
    }
    cout << "Size: " << SIZE << endl;
    runBenchmarks();
    return 0;
}
