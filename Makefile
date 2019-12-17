all:
	g++ -std=c++11 -O3 -c src/instruction.cpp -o bin/instruction.o
	g++ -std=c++11 -O3 -c src/vm.cpp -o bin/vm.o
	g++ -std=c++11 -O3 -c src/oclVM.cpp -o bin/oclVM.o
	g++ -std=c++11 bin/instruction.o bin/vm.o bin/oclVM.o -lOpenCL src/main.cpp -o bin/main
	g++ -std=c++11 bin/instruction.o bin/vm.o bin/oclVM.o -lOpenCL src/testFPGA.cpp -o bin/testFPGA

clean:
	rm -Rf bin 
