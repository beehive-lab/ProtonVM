#ifndef BYTECODES_H
#define BYTECODES_H

enum class Bytecode {
    IADD,
    ISUB,
    IMUL,
    ILT, 
    IEQ, 
    BR, 
    BRT,  // branch if true
    BRF,  // branch if false
    ICONST, 
    LOAD, 
    GLOAD, 
    GSTORE, 
    PRINT, 
    POP, 
    HALT,
    CALL, 
    RET, 

    DUP,    // duplicate the top of the stack
    IDIV,   // integer division
    LSHIFT, // shift to the left (multiply by two)
    RSHIFT, // shift to the right
    ICONST1,  // load constant 1 into the stack
};

#endif