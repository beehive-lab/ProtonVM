all:
	g++ -std=c++11 -O3 -c instruction.cpp 
	g++ -std=c++11 instruction.o main.cpp -o main

clean:
	rm *.o 
	rm main 
