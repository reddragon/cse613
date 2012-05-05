#!/bin/bash

#$ -V
#$ -cwd
#$ -q development
#$ -pe 1way 36
#$ -N distsort-k1
#$ -o output-$JOB_NAME
#$ -e error-$JOB_NAME
#$ -M dhruvbird@gmail.com
#$ -m be
#$ -l h_rt=01:00:00

HW3='/work/01905/rezaul/CSE613/HW3'
OUT_PATH="$WORK/hw3/task-2/k1"
K=10

mkdir -p $OUT_PATH

for i in 1 2 3 4 5 6
do
  echo -ne "Running test $i for k == 1\n" 1>&2
  TEST_PATH="$HW3/tests/test-0$i-in.txt"
  ibrun ./distributed_sort-mpi test.in $K > /dev/null
done
