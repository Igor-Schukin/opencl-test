#!/bin/bash

#PBS -o cltest.out
#PBS -e cltest.err

#cd ./cl-test 
cd ./opencl-test/cltest
./cltest

