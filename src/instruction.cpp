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
    static Instruction i[TOTAL_INSTRUCTIONS];

    i[1] = createInstruction("IADD");
    i[2] = createInstruction("ISUB");
    i[3] = createInstruction("IMUL");
    i[4] = createInstruction("ILT");
    i[5] = createInstruction("IEQ");
    i[6] = createInstruction("IBR");
    i[7] = createInstruction("BRT", 1);
    i[8] = createInstruction("BRF", 1);
    i[9] = createInstruction("ICONST", 1);
    i[10] = createInstruction("LOAD", 1);
    i[11] = createInstruction("GLOAD", 1);
    i[12] = createInstruction("STORE", 1);
    i[13] = createInstruction("GSTORE", 1);
    i[14] = createInstruction("PRINT");
    i[15] = createInstruction("POP");
    i[16] = createInstruction("HALT");
    i[17] = createInstruction("CALL", 3);
    i[18] = createInstruction("RET");
    i[19] = createInstruction("DUP");
    i[20] = createInstruction("IDIV");
    i[21] = createInstruction("LSHITF");
    i[22] = createInstruction("RSHIFT");
    i[23] = createInstruction("ICONST1");

    return i;
} 

void printAllInstructions(Instruction *ins) {
    for (int i = 1; i < TOTAL_INSTRUCTIONS; i++) {
        cout << print(ins[i]) << endl;
    } 
}