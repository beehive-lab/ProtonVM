# ProtonVM: Parallel Bytecode Interpreter For Heterogeneous Hardware 

In a nutshell, ProtoVM is a tiny Parallel Java Bytecode Interpreter (BC) implemented in OpenCL capable of executing on heterogeneous architectures (multi-core CPUs, GPUs and FPGAs).

ProtonVM is a proof-of-concept for running a subset of the Java bytecodes in C++ and OpenCL. The bytecodes defined correspond to an extension of the small-subset 
of Java bytecode explained by Terence Parr, from the University of San Francisco of how to [build a simple Virtual Machine](https://www.youtube.com/watch?v=OjaAToVkoTw). 
This project extends this simple bytecode interpreter to study the feasibility of  running, as efficiently as possible, parallel bytecode interpreters on
heterogeneous computer architectures.

ProtonVM currently runs integer arithmetic only. A possible extension is to include `fp32` and `fp64` math arithmetic and support complex math operations. This way ProtonVM could take 
advantage of the compute-power of modern GPUs, and DSPs available on FPGAs. 

ProtonVM has been presented at [MoreVMs 2020](https://www.youtube.com/watch?v=mok6crMdKgI) and it has been executed on NVIDIA GPUs, Intel HD Graphics and Xilinx FPGAs.

### Bytecode Instructions 

```cpp
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
 
#define THREAD_ID 26                 // load thread-id on top of the stack
#define PARALLEL_GLOAD_INDEXED 27    // load data by accessing device's heap using the thread-id (multi-heap configuration)
#define PARALLEL_GSTORE_INDEXED 28   // store data by accessing device's heap using the thread-id (multi-heap configuration)
```

### Versions of the BC Interpreter

ProtonVM provides different variations of the BC interpreter for testing and experimentation:

* `VM`: this is the baseline BC interpreter implemented in C++. It runs sequentially on the CPU.
* `OCLVM`: Single-thread OpenCL BC interpreter. It is prepared for running a single device thread on the target device. The stack, data and code sections are stored on device's global memory.
* `OCLVMPrivate`:  Single-thread OpenCL BC interpreter. It is prepared for running a single device thread on the target device. The stack is stored in private memory, and data and code sections are stored on device's global memory.
* `OCLVMParallelLoop`: This version of the interpreter is prepared for running with a multi-thread bytecode interpreter exploiting data parallelization. Each thread has its own stack and it performs exactly the same computation across device's threads. The OpenCL kernel is programmed to do the work per thread. Furthermore, this version uses a multi-heap (3 in this case), that allows accessing data in parallel. There are two heaps dedicated to read-only and one for write-only. The stack is stored in private memory, and the heaps are accessed using local memory.


### Example

```cpp
void runParallelOpenCLInterpreter() {
    int size = 1024;
    int groupSize = 16;

    // List of instructions 
    vector<int> vectorMul = {
        THREAD_ID,                  // Push OpenCL Thread-ID into the stack
        DUP,                        // Duplicate the Thread-ID on top of the stack
        PARALLEL_GLOAD_INDEXED, 0,  // Access global memory (e.g., GPU's global memory) using the thread-id and heap index 0
                                    // and push the result (value) on top of the stack
        THREAD_ID,                  // Push OpenCL Thread-ID on top of the stack
        PARALLEL_GLOAD_INDEXED, 1,  // Access global memory (e.g., GPU's global memory) using the thread-id and heap index 1
                                    // and push the result (value) on top of the stack      
        IMUL,                       // Multiply the two elements on top of the stack and push the result
        PARALLEL_GSTORE_INDEXED, 2, // Store the value on top of the stack into global memory (heap 2).
        HALT                        // Stop the program (stack must be empty)
    };

    OCLVMParallelLoop oclVM(vectorMul, 0);     // Create a parallel BC Interpreter and VM
    oclVM.setVMConfig(100, 1024);              // Set sizes for the stack and data sections
    oclVM.setHeapSizes(1024);                  // Set a heap of 1024 int values
    oclVM.setPlatform(0);                      // Select the OpenCL platform 0 for execution
    oclVM.initOpenCL("src/interpreterParallelLoop.cl", false); // Compile the OpenCL BC Interpreter. The second parameter is for FPGA execution. 
    oclVM.initHeap();                          // Init values on the heap
    oclVM.runInterpreter(1024, groupSize);     // Execute the BC interpreter using 1024 threads and groupsize of 16 threads. 
}
```

## How to build?

##### a) Dependencies

```bash
cmake 
GCC >= 9.0
OpenCL >= 1.2 
```

##### b) Compile the project: 
```bash
mkdir build
cd build
cmake .. 
make -j 12
```

##### How to run? 

```bash
$ bin/main
```

## Publications

This work has been presented at [MoreVMs 2020](https://2020.programming-conference.org/details/MoreVMs-2020-papers/7/Running-Parallel-Bytecode-Interpreters-on-Heterogeneous-Hardware).

Link to the paper: [ACM DL](https://dl.acm.org/doi/abs/10.1145/3397537.3397563).

Video presentation: [YouTube](https://www.youtube.com/watch?v=mok6crMdKgI)

Recommended citation:

```bibtex
@inproceedings{10.1145/3397537.3397563,
author = {Fumero, Juan and Stratikopoulos, Athanasios and Kotselidis, Christos},
title = {Running Parallel Bytecode Interpreters on Heterogeneous Hardware},
year = {2020},
isbn = {9781450375078},
publisher = {Association for Computing Machinery},
address = {New York, NY, USA},
url = {https://doi.org/10.1145/3397537.3397563},
doi = {10.1145/3397537.3397563},
booktitle = {Conference Companion of the 4th International Conference on Art, Science, and Engineering of Programming},
pages = {31–35},
numpages = {5},
keywords = {Heterogeneous Architectures, Interpreters, Bytecode, FPGAs, GPUs},
location = {Porto, Portugal},
series = { '20}
}
```

## License 

This project is developed at The University of Manchester, and it is fully open source under the [APACHE 2](https://www.apache.org/licenses/LICENSE-2.0) license.

## Acknowledgments

The work was partially funded by the EU Horizon 2020 [Elegant 957286](https://www.elegant-h2020.eu/) and [E2Data 780245](https://e2data.eu) projects.
