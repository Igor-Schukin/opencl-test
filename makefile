opencl=-I/usr/include/CL -L/usr/lib -lOpenCL

.DEFAULT_GOAL := %
.PHONY: all

%: %.cpp ./lib/opencl.o
	g++ -std=c++17 -I./lib $(opencl) $^ -o $@ 

./lib/opencl.o: ./lib/opencl.cpp ./lib/opencl.h
	g++ -std=c++17 $(opencl) -c ./lib/opencl.cpp -o ./lib/opencl.o
