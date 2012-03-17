#!/bin/bash

#$ -V
#$ -cwd
#$ -q development
#$ -pe 12way 24
#$ -N Cilkview_32768_Development
#$ -o output-$JOB_NAME
#$ -e error-$JOB_NAME
#$ -M gaurav.menghani@gmail.com
#$ -m be
#$ -l h_rt=01:00:00

cilkview -trials all ./a.out 32 ~/hw1/hw1/samples/turn-in/rand-32768-in.txt
