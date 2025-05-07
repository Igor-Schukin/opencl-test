ifeq ($(OS),Windows_NT)
	ext=.exe
else
	ext=
endif

.DEFAULT_GOAL := cltest$(ext)
.PHONY: all

cltest$(ext):cltest.cpp
	g++ cltest.cpp -I/usr/include/CL -L/usr/lib -lOpenCL -o cltest$(ext)
