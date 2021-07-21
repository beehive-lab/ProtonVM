## ParallelBC: Parallel Bytecode Interpreter 

A Parallel Bytecode Interpreter implemented in OpenCL for running on heterogeneous architectures (multi-core CPUs, GPUs and FPGAs).


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

#### How to build?

###### a) Dependencies

```bash
cmake 
GCC >= 9.0
OpenCL >= 1.2 
```

Compile the project: 
```bash
mkdir build
cd build
cmake .. 
make
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
