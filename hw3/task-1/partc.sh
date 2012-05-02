#!/bin/bash

#$ -V
#$ -cwd
#$ -q development
#$ -pe 12way 48
#$ -N hw3-part1c
#$ -o output-$JOB_NAME
#$ -e error-$JOB_NAME
#$ -M dhruvbird@gmail.com
#$ -m be
#$ -l h_rt=01:00:00

HW3='/work/01905/rezaul/CSE613/HW3'
TEST_PATH="$HW3/tests/test-06-in.txt"
OUT_PATH="$WORK/hw3/task-1/c"

mkdir -p $OUT_PATH

for i in 4 16 32 64 128 512 1024 4096 8192
do
  echo -ne "Running test for i == $i\n"
  ./c.out $i < $TEST_PATH > "$OUT_PATH/06-$i.out"
done
