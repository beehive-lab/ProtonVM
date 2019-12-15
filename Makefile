all:
	g++ -std=c++11 -O3 -c instruction.cpp 
	g++ -std=c++11 -O3 -c vm.cpp 
	g++ -std=c++11 instruction.o vm.o main.cpp -o main

clean:
	rm *.o 
	rm main 
