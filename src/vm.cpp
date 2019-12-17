#include <iostream>
#include <string>
#include <vector>
#include "instruction.hpp"
#include "bytecodes.hpp"
#include "vm.hpp"

using namespace std;

VM::VM(vector<int> code, int mainByteCodeIndex) {
    this->code = code;
    this->codeSize = code.size();
    this->ip = mainByteCodeIndex;
    this->ins = createAllInstructions();
}

VM::~VM() {
    if (vmAllocated) {
        this->stack.clear();
        this->data.clear();
    }
}

void VM::runInterpreter() {
    while (ip < codeSize) {
        int opcode = code[ip];
        if (trace) {
            printTrace(opcode);       
        }
        ip++;
        int a, b, c, address, offset, value, numArgs;
        bool doHalt = false;

        switch (opcode) {
            case DUP:
                // Duplicate the stack
                a = stack[sp];
                stack[++sp] = a;
                break;
            case IADD:
                a = stack[sp--];
                b = stack[sp--];
                value = a + b;
                stack[++sp] = value;
                break;
            case ISUB:
                a = stack[sp--];
                b = stack[sp--];
                stack[++sp] = a - b;
                break;    
            case IMUL:
                a = stack[sp--];
                b = stack[sp--];
                stack[++sp] = a * b;
                break;
            case IDIV:
                a = stack[sp--];
                b = stack[sp--];
                stack[++sp] = a / b;
                break;
            case LSHIFT:
                a = stack[sp--];
                a = a << 1;
                stack[++sp] = a;
                break;
            case RSHIFT:
                a = stack[sp--];
                a = a >> 1;
                stack[++sp] = a;
                break;
            case ILT:
                a = stack[sp--];
                b = stack[sp--];
                c = (a < b)? TRUE : FALSE;
                stack[++sp] = c;
                break;
            case IEQ:
                a = stack[sp--];
                b = stack[sp--];
                c = (a == b)? TRUE : FALSE;
                stack[++sp] = c;
                break;
            case BR:
                address = code[ip++];
                ip = address;
                break;
            case BRT:
                address = code[ip++];
                if (stack[sp--] == TRUE) {
                    ip = address;
                }
                break;
            case BRF:
                address = code[ip++];
                if (stack[sp--] == FALSE) {
                    ip = address;
                }
                break;
            case ICONST:
                // load constant into the stack
                value = code[ip++];
                stack[++sp] = value;
                break;
            case ICONST1:
                stack[++sp] = 1;
                break;
            case LOAD:
                address = code[ip++];
                value = stack[fp + address];
                stack[++sp] = value;
                break;
            case GLOAD:
                address = code[ip++];
                value = data[address];
                stack[++sp] = value;
                break;
            case STORE:
                value = stack[sp--];
                address = code[ip++];
                stack[fp + address] = value;
                break;
            case GSTORE:
                value = stack[sp--];
                address = code[ip++];
                data[address] = value;
                break;
            case GLOAD_INDEXED:
                // GLOAD_INDEXED 30 and offset from the stack
                address = code[ip++];
                offset = stack[sp--];
                value = data[(address + offset)];
                stack[++sp] = value;
                break;
            case GSTORE_INDEXED:
                // GSTORE_INDEXED 30
                // value on top of the stack
                // offset, second in the stack
                value = stack[sp--];
                offset = stack[sp--];
                address = code[ip++];
                data[(address + offset)] = value;
                break;
            case PRINT:
                value = stack[sp--];
                std::cout << "[VM] " << value << std::endl;
                break;
            case CALL:
                address = code[ip++];
                numArgs = code[ip++];  // num arguments
                stack[++sp] = numArgs;
                stack[++sp] = fp;
                stack[++sp] = ip;
                fp = sp;
                ip = address;
                break;
            case RET:
                value = stack[sp--];
                sp = fp;
                ip = stack[sp--];
                fp = stack[sp--];
                numArgs = stack[sp--];
                sp -= numArgs;
                stack[++sp] = value;  // return value on top of the stack
                break;
            case POP:
                sp--;
                break;
            case HALT:
                doHalt = true;
                break;
            default:
                cout << "Error" << endl;
                break;
        }

        if (doHalt) {
            break;
        }
    }
}
