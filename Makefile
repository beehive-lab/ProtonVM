all:
	g++ -std=c++11 -O3 -c instruction.cpp -o bin/instruction.o
	g++ -std=c++11 -O3 -c vm.cpp -o bin/vm.o
	g++ -std=c++11 -O3 -c oclVM.cpp -o bin/oclVM.o
	g++ -std=c++11 bin/instruction.o bin/vm.o bin/oclVM.o -lOpenCL main.cpp -o bin/main
	g++ -std=c++11 bin/instruction.o bin/vm.o bin/oclVM.o -lOpenCL testFPGA.cpp -o bin/testFPGA

clean:
	rm *.o 
	rm main 
