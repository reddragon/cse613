#!/bin/bash

#$ -V
#$ -cwd
#$ -q development
#$ -pe 12way 24
#$ -N parallel_bfs_sample02-dev
#$ -o output-$JOB_NAME
#$ -e error-$JOB_NAME
#$ -M dhruvbird@gmail.com
#$ -m be
#$ -l h_rt=01:00:00

HW2='/work/01905/rezaul/CSE613/HW2/samples/'
time ./serial < $HW2/sample-02-in.txt > samples/serial-02-dev.out
time ./parallel < $HW2/sample-02-in.txt > samples/parallel-02-dev.out
time ./parallel_d < $HW2/sample-02-in.txt > samples/parallel_d-02-dev.out
time ./parallel_h < $HW2/sample-02-in.txt > samples/parallel_h-02-dev.out
