all:
	mkdir -p bin
	g++ -std=c++11 -O3 -c src/instruction.cpp -o bin/instruction.o
	g++ -std=c++11 -O3 -c src/vm.cpp -o bin/vm.o
	g++ -std=c++11 -O3 -c src/oclVM.cpp -o bin/oclVM.o
	g++ -std=c++11 bin/instruction.o bin/vm.o bin/oclVM.o -lOpenCL src/main.cpp -o bin/main
	g++ -std=c++11 bin/instruction.o bin/vm.o bin/oclVM.o -lOpenCL src/testFPGA.cpp -o bin/testFPGA
	g++ -std=c++11 bin/instruction.o bin/vm.o bin/oclVM.o -lOpenCL src/gpuBenchmark.cpp -o bin/gpuBenchmark

clean:
	rm -Rf bin 
