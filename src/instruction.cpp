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

#include <iostream>
#include <string>
#include "instruction.hpp"
using namespace std;

Instruction createInstruction(std::string name, int numArguments) {
    Instruction ins;
    ins.name = name;
    ins.numOperarands = numArguments;
    return ins;
}

Instruction createInstruction(std::string name) {
    Instruction ins;
    ins.name = name;
    ins.numOperarands = 0;
    return ins;
}

string print(Instruction ins) {
    string s = "<" + ins.name + ">";
    return s;
}

Instruction* createAllInstructions() {
    static Instruction instructions[TOTAL_INSTRUCTIONS];
    instructions[1] = createInstruction("IADD");
    instructions[2] = createInstruction("ISUB");
    instructions[3] = createInstruction("IMUL");
    instructions[4] = createInstruction("ILT");
    instructions[5] = createInstruction("IEQ");
    instructions[6] = createInstruction("IBR");
    instructions[7] = createInstruction("BRT", 1);
    instructions[8] = createInstruction("BRF", 1);
    instructions[9] = createInstruction("ICONST", 1);
    instructions[10] = createInstruction("LOAD", 1);
    instructions[11] = createInstruction("GLOAD", 1);
    instructions[12] = createInstruction("STORE", 1);
    instructions[13] = createInstruction("GSTORE", 1);
    instructions[14] = createInstruction("PRINT");
    instructions[15] = createInstruction("POP");
    instructions[16] = createInstruction("HALT");
    instructions[17] = createInstruction("CALL", 3);
    instructions[18] = createInstruction("RET");
    instructions[19] = createInstruction("DUP");
    instructions[20] = createInstruction("IDIV");
    instructions[21] = createInstruction("LSHITF");
    instructions[22] = createInstruction("RSHIFT");
    instructions[23] = createInstruction("ICONST1");
    instructions[24] = createInstruction("GLOAD_INDEXED");
    instructions[25] = createInstruction("GSTORE_INDEXED");
    return instructions;
} 

void printAllInstructions(Instruction *ins) {
    for (int i = 1; i < TOTAL_INSTRUCTIONS; i++) {
        cout << print(ins[i]) << endl;
    } 
}