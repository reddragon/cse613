#!/bin/bash

#$ -V
#$ -cwd
#$ -q development
#$ -pe 1way 36
#$ -N distsort-k-bs
#$ -o output-$JOB_NAME
#$ -e error-$JOB_NAME
#$ -M dhruvbird@gmail.com
#$ -m be
#$ -l h_rt=01:00:00

HW3='/work/01905/rezaul/CSE613/HW3'
K=1

for i in 1 2 4 8 16 32 64 128 256 512
do
  echo -ne "Running test 6 for k == $i\n" 1>&2
  TEST_PATH="$HW3/tests/test-02-in.txt"
  ibrun ./distributed_sort-mpi $TEST_PATH $i > /dev/null
done
