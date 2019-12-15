#ifndef VM_HPP
#define VM_HPP

#include <iostream>
#include <string>
#include "instruction.hpp"
#include "bytecodes.hpp"

using namespace std;

class VM {

    public:
        VM(int* code, int mainByteCodeIndex);

        void setVMConfig(int stackSize, int dataSize);

        void setTrace();

        ~VM();

        void printTrace(int opcode);

        void runInterpreter();

    private:
        int* code;
        int* stack;
        int* data;

        int ip;
        int sp = -1;
        int fp;

        int codeSize = 0;

        int TRUE = 1;
        int FALSE = 0;

        bool trace = false;

        bool vmAllocated = false;

        Instruction* ins;

};

#endif 