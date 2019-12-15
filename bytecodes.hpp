#ifndef BYTECODES_H
#define BYTECODES_H

const int IADD = 1;
const int ISUB = 2;
const int IMUL = 3;
const int ILT = 4;
const int IEQ = 5;
const int BR = 6;
const int BRT = 7;  // branch if true
const int BRF = 8;  // branch if false
const int ICONST = 9;
const int LOAD = 10;
const int GLOAD = 11;
const int STORE = 12;
const int GSTORE = 13;
const int PRINT = 14;
const int POP = 15;
const int HALT = 16;
const int CALL = 17;
const int RET = 18;
const int DUP = 19;    // duplicate the top of the stack
const int IDIV = 20;   // integer division
const int LSHIFT = 21; // shift to the left (multiply by two)
const int RSHIFT = 22; // shift to the left (multiply by two)
const int ICONST1 = 23; // load constant 1 into the stack

#endif