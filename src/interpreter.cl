

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

#define TRUE    1
#define FALSE   0

/*
 * Transform int to char on the target device.
 */
int numberToChar(int number, __global char* buffer, int bufferIndex) {
    int n = number;
    int digits[10];
    int counter = 0;
    while (n > 0) {
        int value = n % 10;
        n = n / 10; 
        digits[counter++] = value;
    }

    int i = counter;
    for (; i >= 0; i--) {
        int value = digits[i];
        buffer[bufferIndex++] = '0' + value;
    }
    buffer[bufferIndex++] = '\n';
    return bufferIndex;
}

int printTrace(int opcode, __global char* buffer, int bufferIndex) {
    char valueString[] = {'<', 'O', 'P', '>', ' ', '=', ' '};
    for (int i = 0; i < 7; i++) {
        buffer[bufferIndex++] = valueString[i];
    }
    bufferIndex = numberToChar(opcode, buffer, bufferIndex);
    return bufferIndex;
}

/**
 * OpenCL code for the bytecode interpreter
 */
__attribute__((num_compute_units(1)))
__attribute((reqd_work_group_size(1,1,1)))
__kernel void interpreter(__global int* code, 
                          __global int* stack, 
                          __global int* data, 
                          __global char* buffer, 
                          const int codeSize, 
                          int ip, 
                          int fp, 
                          int sp,
                          int trace) 
{
    char valueString[] = {'[', 'V', 'M', ']', ' ', '=', ' '};
    int bufferIndex = 0;

    while (ip < codeSize) {
        int opcode = code[ip];

        if (trace == 1) {
            bufferIndex = printTrace(opcode, buffer, bufferIndex);       
        }

        ip++;
        int a, b, c, address, value, numArgs, offset;
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
                stack[++sp] = a + b;
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
                c = code[ip++];
                stack[++sp] = c;
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
                address = code[ip++];
                offset = stack[sp--];
                value = data[(address + offset)];
                stack[++sp] = value;
                break;
            case GSTORE_INDEXED:
                value = stack[sp--];
                offset = stack[sp--];
                address = code[ip++];
                data[(address + offset)] = value;
                break;
            case PRINT:
                value = stack[sp--];
                for (int i = 0; i < 7; i++) {
                    buffer[bufferIndex++] = valueString[i];
                }
                bufferIndex = numberToChar(value, buffer, bufferIndex);
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
                doHalt = true;
                break;
        }
        if (doHalt) {
            break;
        }
    }
}