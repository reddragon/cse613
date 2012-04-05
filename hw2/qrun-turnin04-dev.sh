#!/bin/bash

#$ -V
#$ -cwd
#$ -q development
#$ -pe 12way 24
#$ -N parallel_bfs_turnin04-dev
#$ -o output-$JOB_NAME
#$ -e error-$JOB_NAME
#$ -M dhruvbird@gmail.com
#$ -m be
#$ -l h_rt=03:00:00

HW2='/work/01905/rezaul/CSE613/HW2/samples/turn-in/'
time ./serial     < $HW2/test-04-in.txt > turn-in/test-04-1a-out.txt
time ./parallel   < $HW2/test-04-in.txt > turn-in/test-04-1b-out.txt
time ./parallel_d < $HW2/test-04-in.txt > turn-in/test-04-1d-out.txt
time ./parallel_h < $HW2/test-04-in.txt > turn-in/test-04-1h-out.txt
