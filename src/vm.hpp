#ifndef VM_HPP
#define VM_HPP

#include <iostream>
#include <string>
#include <vector>
#include "instruction.hpp"
#include "bytecodes.hpp"
#include "abstractVM.hpp"

using namespace std;

class VM : public AbstractVM {

    public:
        VM(vector<int> code, int mainByteCodeIndex);

        ~VM();

        // Implementation of the Interpreter in C++
        void runInterpreter();
};

#endif 