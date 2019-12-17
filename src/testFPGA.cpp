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

#define SIZE 1000

void runBenchmarkCplus() {
    // Vector addition in a LOOP
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

void runBenchmarkOpenCL() {
    // Vector addition in a LOOP
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
            IADD,
            GSTORE_INDEXED, 0,
            ICONST1,
            IADD,
            BR, 2,
            POP,
            HALT
    };

    vector<long> totalTime;

    OCLVM oclVM(vectorAdd, 0);
    oclVM.setVMConfig(100, SIZE * 3);
    oclVM.setPlatform(1);
    oclVM.initOpenCL("src/build/mykerinterpreternel.xclbin", true);
    for (int i = 0; i < 11; i++) {    
        oclVM.initHeap();
        oclVM.runInterpreter();
        long kernelTime = oclVM.getKernelTime();
        cout << "Kernel Time: " << kernelTime << endl;
        totalTime.push_back(kernelTime);
    }

    double medianTotalTime = median(totalTime);
    cout << "Median OpenCLTimer: " << medianTotalTime << endl;
}

void runBenchmarks() {
    runBenchmarkCplus();
    runBenchmarkOpenCL();
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
    runBenchmarks();
    return 0;
}
