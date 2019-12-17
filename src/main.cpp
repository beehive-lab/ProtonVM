#include <string>
#include <iostream>
#include <vector>
using namespace std;

#include "instruction.hpp"
#include "bytecodes.hpp"
#include "vm.hpp"
#include "oclVM.hpp"

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

int main(int argc, char** argv) {
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

    return 0;
}