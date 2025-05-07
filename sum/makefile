ifeq ($(OS),Windows_NT)
	ext=.exe
else
	ext=
endif

opencl=-I/usr/include/CL -L/usr/lib -lOpenCL

.DEFAULT_GOAL := %$(ext)
.PHONY: all

%$(ext): %.cpp opencl.o
	g++ $(opencl) $^ -o $@

opencl.o: opencl.cpp opencl.h
	g++ $(opencl) -c opencl.cpp -o opencl.o
