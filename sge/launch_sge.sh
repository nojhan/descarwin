#!/bin/bash
# SGE options
#$ -cwd
#$ -j y
#$ -N sge_log
#$ -S /bin/bash
#$ -q all.q@@dragon
#$ -pe make 10
# MPI options
IMPL=openmpi
MPIRUN=/home/bouvier/${IMPL}/bin/mpirun
# DAE options
RUNS=2
GEN_MAX=1
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
mv "sge_log.o${JOB_ID}" "plans${JOB_ID}/"

