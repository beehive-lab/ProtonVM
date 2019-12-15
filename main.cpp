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


int main(int argc, char** argv) {
    testHello();
    testProgram1();
    testProgram2();
    return 0;
}