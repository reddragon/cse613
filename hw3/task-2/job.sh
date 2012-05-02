#!/bin/bash

#$ -V
#$ -cwd
#$ -q development
#$ -pe 8way 48
#$ -N MPI
#$ -o output-$JOB_NAME
#$ -e error-$JOB_NAME
#$ -M gaurav.menghani@gmail.com
#$ -m be
#$ -l h_rt=01:00:00

ibrun ./distributed_sort-mpi
