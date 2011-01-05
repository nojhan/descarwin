#!/bin/bash
#####################
# GENERIC FUNCTIONS #
#####################

# TODO add ipc3

# submit arrays of jobs with SGE, on each instance available in a given directory
function sge_submit_array_ipc()
{

    # directory where to search PDDL domains and instance files 
    base_rep=$1

    # nb of differents runs for each instance
    runs=$2

    # directory where to put everything
    res=$3

    options=$4

    # create directories if they do not exists
    mkdir -p $res
    mkdir -p $res/data
    mkdir -p $res/logs
    mkdir -p $res/status
    mkdir -p $res/plans
    mkdir -p $res/gens

    # list the available domain files in the sub-tree of this directory
    # default: do not consider ADL
    # domains=`find $base_rep -wholename "*/SimpleTime/*.pddl" -print | grep -v "SimpleTime.*[0-9]" | grep -v adl`
    #domains=`find $base_rep -wholename "*/Strips/*.pddl" -print | grep -v adl`
    # domains=`find $base_rep -wholename "*/Strips/*.pddl" -print | grep -v adl ; find $base_rep -wholename "*/SimpleTime/*.pddl" -print | grep -v "SimpleTime.*[0-9]" | grep -v adl`

    domains=""
    if [ $options == 0 ] ; then
	domains=`find $base_rep -wholename "*/SimpleTime/*.pddl" -print | grep -v "SimpleTime.*[0-9]" | grep -v adl`
    else
	domains=`find $base_rep -wholename "*/Strips/*.pddl" -print | grep -v adl`
    fi
    
    for domain in $domains; do

        cptr=0
	
	for i in `seq 20`; do

	    instance=`dirname $domain`/pfile$i
	    if test -r $instance && test -r $domain; then

            # parent directory
		d=`basename $domain .pddl`
		echo $d
		echo $i
            # submit an array of *runs jobs
            # additional arguments are in $options
		cmd1="qsub -l h_rt=00:30:00 -q all.q@@ls -b y -N dae_${d}_${i}      -cwd -o $res/data/ -e $res/logs/ -t 1-$runs -S /bin/bash ./run_wrapper.sh $domain $instance $options $res"
		cmd2="qsub -l h_rt=00:30:00 -q all.q@@ls -b y -N jack_${d}_${i} -cwd -o $res/data/ -e $res/logs/ -t 1-$runs -S /bin/bash ./run_wrapper_jack.sh $domain $instance $options $res"
		
		echo $cmd1
		echo $cmd2
            #./run_wrapper.sh $domain $instance $options
            #./run_wrapper_jack.sh $domain $instance $options
		$cmd1
		$cmd2

		cptr=$((cptr+1))
	    else
		echo "ERROR, instance or domain files cannot be open: $instance"
	    fi
	done # i
    done # domain

    echo "$((cptr*runs)) runs submitted"
}


#######################
# CURRENT EXPERIMENTS #
#######################

# nb of differents runs for each instance
runs=11

# IPC3

sge_submit_array_ipc "/tools/pddl/ipc/IPC3/" $runs "tempo-sat" 0 
sge_submit_array_ipc "/tools/pddl/ipc/IPC3/" $runs "seq-sat" 1

