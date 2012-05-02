#!/bin/bash

#$ -V
#$ -cwd
#$ -q development
#$ -pe 12way 48
#$ -N hw3-part1d
#$ -o output-$JOB_NAME
#$ -e error-$JOB_NAME
#$ -M dhruvbird@gmail.com
#$ -m be
#$ -l h_rt=01:00:00

HW3='/work/01905/rezaul/CSE613/HW3'
TEST_PATH="$HW3/tests/test-06-in.txt"
OUT_PATH="$WORK/hw3/task-1/d"

mkdir -p $OUT_PATH

for i in 1 2 3 4 5 6
do
  echo -ne "Running test for i == $i (parallel followed by serial)\n" 1>&2
  ./c.out 4096 < $TEST_PATH > /dev/null
  ./sc.out 4096 < $TEST_PATH > /dev/null
done
