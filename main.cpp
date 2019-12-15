#include <string>
#include <iostream>
using namespace std;

#include "instruction.hpp"
#include "bytecodes.hpp"
#include "vm.hpp"

void testHello() {
    int hello[] = {
        ICONST, 128,
        ICONST, 1,
        IADD,
        GSTORE, 0,
        GLOAD, 0,
        PRINT,
        HALT
    };
    Instruction* ins = createAllInstructions();
    VM vm(hello,  0);
    vm.setVMConfig(100, 100);
    vm.setTrace();
    vm.runInterpreter();
}

void testProgram1() {
    int program1[] = {
        ICONST, 100,
        ICONST, 100,
        IADD,
        GSTORE, 2,
        GLOAD, 2,
        PRINT,
        HALT
    };
    Instruction* ins = createAllInstructions();
    VM vm(program1,  0);
    vm.setVMConfig(100, 100);
    vm.setTrace();
    vm.runInterpreter();
}

void testProgram2() {
    int program2[] = {
        ICONST, 100,
        LSHIFT,
        DUP,
        PRINT,
        RSHIFT,
        PRINT,
        HALT
    };
    Instruction* ins = createAllInstructions();
    VM vm(program2,  0);
    vm.setVMConfig(100, 100);
    vm.setTrace();
    vm.runInterpreter();
}

void testFunction() {
    int functionCall[] = {
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
    Instruction* ins = createAllInstructions();
    VM vm(functionCall,  6); // Start address is 6
    vm.setVMConfig(100, 100);
    vm.setTrace();
    vm.runInterpreter();
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
    
    return 0;
}