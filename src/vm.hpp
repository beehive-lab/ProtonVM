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