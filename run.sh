#!/bin/bash

prog=$1

echo "=== compiling $prog... ==="
module load compilers/gcc/gcc-11.2.0-nvptx
make $prog

if [ $? -eq 0 ]; then
    echo "=== starting $prog... ==="
    echo "" >$prog.out
    qsub -v prog=$prog -l nodes=1:ppn=1:gpus=1 ./job.sh
    tail -f $prog.out
else
    echo "=== compilation is finished with errors ==="
fi
