#!/bin/bash
#####################
# GENERIC FUNCTIONS #
#####################

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

    # list the available domain files in the sub-tree of this directory
    # default: do not consider ADL
    # domains=`find $base_rep -wholename "*/SimpleTime/*.pddl" -print | grep -v "SimpleTime.*[0-9]" | grep -v adl`
    #domains=`find $base_rep -wholename "*/Strips/*.pddl" -print | grep -v adl`
    # domains=`find $base_rep -wholename "*/Strips/*.pddl" -print | grep -v adl ; find $base_rep -wholename "*/SimpleTime/*.pddl" -print | grep -v "SimpleTime.*[0-9]" | grep -v adl`

    domains=""
    if [ $options == 0 ] ; then
	domains=`find $base_rep -wholename "./DOMAINS/AIRPORT/TEMPORAL*/*_DOMAIN.PDDL"`
    else
	domains=`find $base_rep -wholename "./DOMAINS/AIRPORT/NONTEMPORAL*/STRIPS/*_DOMAIN.PDDL";find $base_rep -wholename "./DOMAINS/PSR/STRIPS/*_DOMAIN.PDDL"; grep -v adl;find $base_rep -wholename "./DOMAINS/PROMELA/STRIPS/*_DOMAIN.PDDL"`
    fi
    
    for domain in $domains; do

        cptr=0
	
	instances=`find $base_rep -wholename "./DOMAINS/AIRPORT/TEMPORAL*/*" | grep -v DOMAIN`
	for instance in $instances; do

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
#		$cmd1
#		$cmd2

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


# IPC4 

# AIRPORT
# domain=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/NONTEMPORAL/STRIPS/P01_DOMAIN.PDDL to P50_DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/NONTEMPORAL/STRIPS/P01_*.PDDL to P50_*.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/NONTEMPORAL/STRIPS" 1 $runs "seq-sat"

# domain=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL/STRIPS_TEMPORAL/P01_DOMAIN.PDDL to P50_DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL/STRIPS_TEMPORAL/P01_*.PDDL to P50_*.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL/STRIPS_TEMPORAL" 0 $runs "tempo-sat"

# domain=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL_TIMEWINDOWS/STRIPS_TEMPORAL_TIMEDLITERALS/P01_DOMAIN.PDDL to P50_DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL_TIMEWINDOWS/STRIPS_TEMPORAL_TIMEDLITERALS/P01_*.PDDL to P50_*.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL_TIMEWINDOWS/STRIPS_TEMPORAL_TIMEDLITERALS" 0 $runs "tempo-sat"

# domain=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL_TIMEWINDOWS_COMPILED/STRIPS_TEMPORAL/P01_DOMAIN.PDDL to P50_DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL_TIMEWINDOWS_COMPILED/STRIPS_TEMPORAL/P01_*.PDDL to P50_*.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL_TIMEWINDOWS_COMPILED/STRIPS_TEMPORAL" 0 $runs "tempo-sat"


# PIPESWORLD
# domain=/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/NOTANKAGE_NONTEMPORAL/STRIPS/DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/NOTANKAGE_NONTEMPORAL/STRIPS/P01_*.PDDL to P50_*.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/NOTANKAGE_NONTEMPORAL/STRIPS" 1 $runs "seq-sat"

# domain=/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/TANKAGE_NONTEMPORAL/STRIPS/DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/TANKAGE_NONTEMPORAL/STRIPS/P01_*.PDDL to P50_*.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/TANKAGE_NONTEMPORAL/STRIPS" 1 $runs "seq-sat"

# domain=/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/TANKAGE_TEMPORAL/STRIPS/DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/TANKAGE_TEMPORAL/STRIPS/P01_*.PDDL to P50_*.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/TANKAGE_TEMPORAL/STRIPS" 0 $runs "tempo-sat"

# PROMELA
# domain=/tools/pddl/ipc/IPC4/DOMAINS/PROMELA/OPTICAL_TELEGRAPH/STRIPS/P01_DOMAIN*.PDDL to P14_DOMAIN*.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/PROMELA/OPTICAL_TELEGRAPH/STRIPS/P01_OPT*.PDDL to P14_OPT*.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/PROMELA/OPTICAL_TELEGRAPH/STRIP" 1 $runs "seq-sat"

# domain=/tools/pddl/ipc/IPC4/DOMAINS/PROMELA/PHILOSOPHERS/STRIPS/P01_DOMAIN*.PDDL to P29_DOMAIN*.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/PROMELA/PHILOSOPHERS/STRIPS/P01_PHIL*.PDDL to P29_PHIL*.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/PROMELA/PHILOSOPHERS/STRIP" 1 $runs "seq-sat"

# PSR
# domain=/tools/pddl/ipc/IPC4/DOMAINS/PSR/SMALL/STRIPS/P01_DOMAIN.PDDL to P50_DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/PSR/SMALL/STRIPS/P01_*.PDDL to P50_*.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/PSR/SMALL/STRIPS" 1 $runs "seq-sat"

# SATELLITE
# domain=/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/STRIPS/STRIPS/DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/STRIPS/STRIPS/P01_*.PDDL to P36_*.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/STRIPS/STRIPS" 1 $runs "seq-sat"

# domain=/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/TIME/STRIPS_TEMPORAL/DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/TIME/STRIPS_TEMPORAL/P01_*.PDDL to P36_*.PDDL
#sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/TIME/STRIPS_TEMPORAL0 $runs "tempo-sat"

