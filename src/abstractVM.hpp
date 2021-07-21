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

        virtual void initHeap() {
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