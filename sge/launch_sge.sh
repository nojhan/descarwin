#!/bin/bash

# SGE options
# use current directory
#$ -cwd
# join output and error log
#$ -j y
# call job sge_mpi (output file will be named sge_mpi.o{JOB_ID})
#$ -N sge_mpi
# use bash
#$ -S /bin/bash
# use queue dragon (i.e all the dragon machines)
#$ -q all.q@@dragon
# creates 10 hosts
#$ -pe make 10
# limit host access to this job (so as to avoid issues relative to multitask)
#$ -l exclusive=true

# MPI options
IMPL=openmpi
MPIRUN=/home/bouvier/${IMPL}/bin/mpirun

# DAE options
RUNS=2
GEN_MAX=3
MODE=release # release or debug
DOMAIN=/home/bouvier/descarwin_examples/elevators/domain/domain.pddl
INSTANCE=/home/bouvier/descarwin_examples/elevators/problems/p01.pddl
let SEED=${JOB_ID}*${RANDOM}

echo "MPI DAE"
echo "*******"
echo "Launching ${RUNS} runs in ${MODE} mode."
echo "Seed before parallelization is ${SEED}."

mkdir "plans${JOB_ID}"
(cd "plans${JOB_ID}" && ${MPIRUN} ../../${MODE}/dae/dae --domain=${DOMAIN} --instance=${INSTANCE} --gen-max=${GEN_MAX} --gen-min=1 --runs-max=${RUNS} --seed=${SEED})
mv "sge_mpi.o${JOB_ID}" "plans${JOB_ID}/"

