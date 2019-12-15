#ifndef ABSTRACT_VM_HPP
#define ABSTRACT_VM_HPP

#include <iostream>
#include <string>
#include "instruction.hpp"
#include "bytecodes.hpp"

using namespace std;

class AbstractVM {

    public:

        void setVMConfig(int stackSize, int dataSize) {
            this->stack = new int[stackSize];
            this->data = new int[dataSize];
            vmAllocated = true;
        }

        void setTrace() {
            this->trace = true;
        }

        void printTrace(int opcode) {
            Instruction instruction = ins[opcode];
            cout << print(instruction) << " ";
            if (instruction.numOperarands == 1) {
                cout << code[ip + 1];
            } else if (instruction.numOperarands == 2) {
                cout << code[ip + 1] << " " << code[ip + 2];
            }
            cout << endl;
        }

        virtual void runInterpreter() = 0;

    protected:
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