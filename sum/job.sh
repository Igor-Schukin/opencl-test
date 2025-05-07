#!/bin/bash

#PBS -o $prog.out
#PBS -e $prog.err

cd ./$dir
start=$(date +%s%3N)
./$prog
end=$(date +%s%3N)
elapsed=$((end - start))
echo "Elapsed time: ${elapsed} ms"