/**
 * OpenCL interpreter for running a subset of Java bytecodes.
 */

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

/**
 * OpenCL code for the bytecode interpreter.
 * Multi-heap
 */
 __attribute__((reqd_work_group_size(16,1,1)))
__kernel void interpreter(__constant int* code, 
                          __global int* data1, 
                          __global int* data2, 
                          __global int* data3, 
                          __global char* buffer, 
                          const int codeSize, 
                          int ip, 
                          int fp, 
                          int sp,
                          int trace) 
{

    int idx = get_global_id(0);

    // Stack in private memory
    __private int stack[100];

    // Heaps in local memory
    __local int privateHeap1[16];
    __local int privateHeap2[16];
    __local int privateHeap3[16];

    int lid = get_local_id(0);

    privateHeap1[lid] = data1[idx];
    privateHeap2[lid] = data2[idx];
    privateHeap3[lid] = data3[idx];
    // Wait for all threads within the workwroup
    barrier(CLK_LOCAL_MEM_FENCE);

    while (ip < codeSize) {
        int opcode = code[ip];
        ip++;
        int a, b, c, address, value, numArgs, offset, heapNumber;
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
                value = data1[address];
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
                data1[address] = value;
                break;
            case GLOAD_INDEXED:
                address = code[ip++];
                offset = stack[sp--];
                value = data1[(address + offset)];
                stack[++sp] = value;
                break;
            case PARALLEL_GLOAD_INDEXED:
                heapNumber = code[ip++];
                offset = stack[sp--];
                switch (heapNumber) {
                    case 0:
                        value = privateHeap1[offset];      
                        break;
                    case 1:
                        value = privateHeap2[offset];      
                        break;
                    case 2:
                        value = privateHeap3[offset];      
                        break;
                }
                stack[++sp] = value;
                break;
            case GSTORE_INDEXED:
                value = stack[sp--];
                offset = stack[sp--];
                address = code[ip++];
                data1[(address + offset)] = value;
                break;
            case PARALLEL_GSTORE_INDEXED:
                value = stack[sp--];
                offset = stack[sp--];
                heapNumber = code[ip++];
                switch (heapNumber) {
                    case 0:
                        privateHeap1[offset] = value;
                        break;
                    case 1:
                        privateHeap2[offset] = value;
                        break;
                    case 2:
                        privateHeap3[offset] = value;
                        break;
                }
                break;
            case PRINT:
                value = stack[sp--];
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
            case THREAD_ID:
                value = get_local_id(0);
                stack[++sp] = value;
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

    // Copy to global memory
    data1[idx] = privateHeap1[lid];
    data2[idx] = privateHeap2[lid];
    data3[idx] = privateHeap3[lid];
}   