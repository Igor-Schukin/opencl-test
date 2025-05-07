#!/bin/bash

echo "=== compiling... ==="
module load compilers/gcc/gcc-11.2.0-nvptx
make

if [ $? -eq 0 ]; then
    echo "=== starting job... ==="
    echo "" >cltest.out
    qsub -l nodes=1:ppn=1:gpus=1 ./cltest.sh
    tail -f cltest.out
else
    echo "=== compilation is finished with errors ==="
fi
