#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>

#define TOTAL_INSTRUCTIONS 26

struct Instruction {
    std::string name;
    int numOperarands;
};

Instruction createInstruction(std::string name, int numArguments);

Instruction createInstruction(std::string name);

std::string print(Instruction ins);

Instruction* createAllInstructions();

void printAllInstructions(Instruction *ins);

#endif