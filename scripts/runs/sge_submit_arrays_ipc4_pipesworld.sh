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

    # additional options to give to the solver (e.g. --sequential)
    options=$2

    # nb of differents runs for each instance
    runs=$3

    # directory where to put everything
    res=$4

    # create directories if they do not exists
    mkdir -p $res
    mkdir -p $res/data
    mkdir -p $res/logs
    mkdir -p $res/status
    mkdir -p $res/plans
    mkdir -p $res/gens

    if [ $options == 0 ] ; then
	echo "ERROR, the temporal domain requires fluents"
    else
	domain=`$base_rep/STRIPS/DOMAIN.PDDL`
        # list the available instance files in the sub-tree of this directory
	instances=`find $base_rep -name "*" | grep -v "DOMAIN"`

	for i in $instances; do
	    
	    cptr=0

	    if test -r $i && test -r $domain; then

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
#		$cmd1
#		$cmd2

		cptr=$((cptr+1))
	    else
		echo "ERROR, instance or domain files cannot be open: $instance"
	    fi
	done # instance
    fi
	echo "$((cptr*runs)) runs submitted"
}


#######################
# CURRENT EXPERIMENTS #
#######################

# nb of differents runs for each instance
runs=11


# IPC4 STRIPS
sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/NOTANKAGE_NONTEMPORAL" 1 $runs "seq-sat"

