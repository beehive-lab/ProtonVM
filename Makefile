all:
	g++ -std=c++11 -O3 -c instruction.cpp 
	g++ -std=c++11 -O3 -c vm.cpp 
	g++ -std=c++11 -O3 -c oclVM.cpp 
	g++ -std=c++11 instruction.o vm.o oclVM.o -lOpenCL main.cpp -o main
	g++ -std=c++11 instruction.o vm.o oclVM.o -lOpenCL testFPGA.cpp -o testFPGA

clean:
	rm *.o 
	rm main 
