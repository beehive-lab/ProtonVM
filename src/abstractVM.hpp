#ifndef ABSTRACT_VM_HPP
#define ABSTRACT_VM_HPP

#include <iostream>
#include <string>
#include <vector>
#include "instruction.hpp"
#include "bytecodes.hpp"

using namespace std;

class AbstractVM {

    public:

        void setVMConfig(int stackSize, int dataSize) {
            this->stack.resize(stackSize);
            this->data.resize(dataSize);
            this->stackSize = stackSize;
            this->dataSize = dataSize;
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

            cout << "\t[";
            for (auto i = 0; i <= sp; i++) {
                cout << stack[i] << ' ';
            }
            
            cout << "]" << endl;
        }

        void initHeap() {
            for (auto i = 0; i < data.size(); i++) {
                data[i] = i;
            }
        }

        void printHeap() {
            cout << "HEAP: " << endl;
            for (auto i = data.begin(); i != data.end(); i++) {
                std::cout << *i << ' ';
            }
        }

        virtual void runInterpreter() = 0;

    protected:
        vector<int> code;
        vector<int> stack;
        vector<int> data;

        int codeSize;
        int stackSize;
        int dataSize;

        int ip = 0;
        int sp = -1;
        int fp = 0;

        bool trace = false;

        bool vmAllocated = false;

        Instruction* ins;

};

#endif