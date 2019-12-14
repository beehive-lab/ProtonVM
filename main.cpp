#include <string>
#include <iostream>
using namespace std;

#include "instruction.hpp"


int main(int argc, char** argv) {

    Instruction* ins = createAllInstructions();
    printAllInstructions(ins);
    
    return 0;
}