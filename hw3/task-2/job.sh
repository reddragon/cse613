#!/bin/bash

#$ -V
#$ -cwd
#$ -q development
#$ -pe 12way 36
#$ -N MPI
#$ -o output-$JOB_NAME
#$ -e error-$JOB_NAME
#$ -M gaurav.menghani@gmail.com
#$ -m be
#$ -l h_rt=01:00:00

ibrun ./distributed_sort-mpi test.in 4 >> t.out
ibrun ./distributed_sort-mpi test.in 8 >> t.out
ibrun ./distributed_sort-mpi test.in 16 >> t.out
ibrun ./distributed_sort-mpi test.in 32 >> t.out
ibrun ./distributed_sort-mpi test.in 64 >> t.out
ibrun ./distributed_sort-mpi test.in 128 >> t.out
ibrun ./distributed_sort-mpi test.in 256 >> t.out
ibrun ./distributed_sort-mpi test.in 512 >> t.out
