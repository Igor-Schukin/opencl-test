ifeq ($(OS),Windows_NT)
	ext=.exe
else
	ext=
endif

opencl=-I/usr/include/CL -L/usr/lib -lOpenCL

.DEFAULT_GOAL := %$(ext)
.PHONY: all

%$(ext): %.cpp ../lib/opencl.o
	g++ $(opencl) $^ -o $@ -h ../lib

../lib/opencl.o: ../lib/opencl.cpp ../lib/opencl.h
	g++ $(opencl) -c ../lib/opencl.cpp -o ../lib/opencl.o
