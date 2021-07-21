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

#ifndef BYTECODES_H
#define BYTECODES_H

#define IADD     1
#define ISUB     2
#define IMUL     3
#define ILT      4
#define IEQ      5
#define BR       6
#define BRT      7   // branch if true
#define BRF      8   // branch if false
#define ICONST   9
#define LOAD    10
#define GLOAD   11
#define STORE   12
#define GSTORE  13
#define PRINT   14
#define POP     15
#define HALT    16
#define CALL    17
#define RET     18
#define DUP     19   // duplicate the top of the stack
#define IDIV    20   // integer division
#define LSHIFT  21   // shift to the left (multiply by two)
#define RSHIFT  22   // shift to the left (multiply by two)
#define ICONST1 23   // load constant 1 into the stack

#define GLOAD_INDEXED  24  // top_stack <- global[top-stack]
#define GSTORE_INDEXED 25

#define THREAD_ID 26
#define PARALLEL_GLOAD_INDEXED 27
#define PARALLEL_GSTORE_INDEXED 28

#define TRUE    1
#define FALSE   0

#endif