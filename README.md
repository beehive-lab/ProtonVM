## Parallel Bytecode Interpreter 

A Parallel Bytecode Interpreter implemented in OpenCL for running on heterogenous architectures (multi-core CPUs, GPUs and FPGAs).



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

```bash
$ make 
mkdir -p bin
g++ -std=c++11 -O3 -c src/instruction.cpp -o bin/instruction.o
g++ -std=c++11 -O3 -c src/vm.cpp -o bin/vm.o
g++ -std=c++11 -O3 -c src/oclVM.cpp -o bin/oclVM.o
g++ -std=c++11 bin/instruction.o bin/vm.o bin/oclVM.o -lOpenCL src/main.cpp -o bin/main
g++ -std=c++11 bin/instruction.o bin/vm.o bin/oclVM.o -lOpenCL src/testFPGA.cpp -o bin/testFPGA
g++ -std=c++11 bin/instruction.o bin/vm.o bin/oclVM.o -lOpenCL src/gpuBenchmark.cpp -o bin/gpuBenchmark
```

##### How to run? 

```bash
$ bin/main
```
