#!/bin/bash
#####################
# GENERIC FUNCTIONS #
#####################

# submit arrays of jobs with SGE, on each instance available in a given directory
function sge_submit_array_ipc()
{
    base_rep=$1 # directory where to search PDDL domains and instance files 
    runs=$2 # nb of differents runs for each instance
    res=$3 # directory where to put everything
    seq=$4
    domains=`find $base_rep -wholename "*_DOMAIN.PDDL"` # list the available domain files in the sub-tree of this directory
    
    for domain in $domains; do
        cptr=0
	pattern=`echo $domain | sed s/_DOMAIN/\*/`
	instance=`ls $pattern | grep -v _DOMAIN`
	if test -r $instance && test -r $domain; then
            # create directory tree
	    respath=$res/`dirname $domain | sed "s/\/tools\/pddl\/ipc\///"`
            # create directories if they do not exists
	    mkdir -p $respath
	    mkdir -p $respath/data
	    mkdir -p $respath/logs
	    mkdir -p $respath/status
	    mkdir -p $respath/plans
	    mkdir -p $respath/gens
            # submit an array of *runs jobs
	    cmd1="qsub -l h_rt=00:30:00 -q all.q@@ls -b y -N dae      -cwd -o $respath/data/ -e $respath/logs/ -t 1-$runs -S /bin/bash ./run_wrapper.ipc4.sh $domain $instance $respath $seq"
#	    cmd2="qsub -l h_rt=00:30:00 -q all.q@@ls -b y -N jack -cwd -o $respath/data/ -e $respath/logs/ -t 1-$runs -S /bin/bash ./run_wrapper_jack.ipc4.sh $domain $instance $respath"
	    echo $cmd1
#	    echo $cmd2
#            ./run_wrapper.ipc4.sh $domain $instance $respath
#            ./run_wrapper_jack.ipc4.sh $domain $instance $respath
	    $cmd1
#	    $cmd2

	    cptr=$((cptr+1))
	else
	    echo "ERROR, instance or domain files cannot be open: $instance"
	fi
    done # domain
    echo "$((cptr*runs)) runs submitted"
}

function sge_submit_array_ipc_single_domain()
{
    base_rep=$1 # directory where to search PDDL domains and instance files 
    runs=$2 # nb of differents runs for each instance
    res=$3 # directory where to put everything
    domain=$base_rep/DOMAIN.PDDL
    instances=`ls $base_rep/P*.PDDL`
    for instance in $instances; do
        cptr=0
	if test -r $instance && test -r $domain; then
            # create directory tree
	    respath=$res/`dirname $domain | sed "s/\/tools\/pddl\/ipc\///"`
            # create directories if they do not exists
	    mkdir -p $respath
	    mkdir -p $respath/data
	    mkdir -p $respath/logs
	    mkdir -p $respath/status
	    mkdir -p $respath/plans
	    mkdir -p $respath/gens
            # submit an array of *runs jobs
	    cmd1="qsub -l h_rt=00:35:00 -q all.q@@ls -b y -N dae      -cwd -o $respath/data/ -e $respath/logs/ -t 1-$runs -S /bin/bash ./run_wrapper.ipc4.sh $domain $instance $respath $seq"
#	    cmd2="qsub -l h_rt=00:30:00 -q all.q@@ls -b y -N jack -cwd -o $respath/data/ -e $respath/logs/ -t 1-$runs -S /bin/bash ./run_wrapper_jack.ipc4.sh $domain $instance $respath"
	    echo $cmd1
#	    echo $cmd2
#            ./run_wrapper.ipc4.sh $domain $instance $respath
#            ./run_wrapper_jack.ipc4.sh $domain $instance $respath
	    $cmd1
#	    $cmd2
	    cptr=$((cptr+1))
	else
	    echo "ERROR, instance or domain files cannot be open: $instance"
	fi
    done # domain
    echo "$((cptr*runs)) runs submitted"
}


#######################
# CURRENT EXPERIMENTS #
#######################

runs=11 # nb of differents runs for each instance

# IPC4 

# AIRPORT
# domain=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/NONTEMPORAL/STRIPS/P01_DOMAIN.PDDL to P50_DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/NONTEMPORAL/STRIPS/P01_*.PDDL to P50_*.PDDL
sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/NONTEMPORAL/STRIPS" $runs "seq-sat" 1

# domain=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL/STRIPS_TEMPORAL/P01_DOMAIN.PDDL to P50_DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL/STRIPS_TEMPORAL/P01_*.PDDL to P50_*.PDDL
sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL/STRIPS_TEMPORAL" $runs "tempo-sat" 0

# domain=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL_TIMEWINDOWS/STRIPS_TEMPORAL_TIMEDLITERALS/P01_DOMAIN.PDDL to P50_DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL_TIMEWINDOWS/STRIPS_TEMPORAL_TIMEDLITERALS/P01_*.PDDL to P50_*.PDDL
sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL_TIMEWINDOWS/STRIPS_TEMPORAL_TIMEDLITERALS" $runs "tempo-sat" 0

# domain=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL_TIMEWINDOWS_COMPILED/STRIPS_TEMPORAL/P01_DOMAIN.PDDL to P50_DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL_TIMEWINDOWS_COMPILED/STRIPS_TEMPORAL/P01_*.PDDL to P50_*.PDDL
sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/AIRPORT/TEMPORAL_TIMEWINDOWS_COMPILED/STRIPS_TEMPORAL" $runs "tempo-sat" 0

# PIPESWORLD
# domain=/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/NOTANKAGE_NONTEMPORAL/STRIPS/DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/NOTANKAGE_NONTEMPORAL/STRIPS/P01_*.PDDL to P50_*.PDDL
sge_submit_array_ipc_single_domain "/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/NOTANKAGE_NONTEMPORAL/STRIPS" $runs "seq-sat" 1

# domain=/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/TANKAGE_NONTEMPORAL/STRIPS/DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/TANKAGE_NONTEMPORAL/STRIPS/P01_*.PDDL to P50_*.PDDL
sge_submit_array_ipc_single_domain "/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/TANKAGE_NONTEMPORAL/STRIPS" $runs "seq-sat" 1

# domain=/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/TANKAGE_TEMPORAL/STRIPS_TEMPORAL/DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/TANKAGE_TEMPORAL/STRIPS_TEMPORAL/P01_*.PDDL to P50_*.PDDL
sge_submit_array_ipc_single_domain "/tools/pddl/ipc/IPC4/DOMAINS/PIPESWORLD/TANKAGE_TEMPORAL/STRIPS_TEMPORAL" $runs "tempo-sat" 0

# PROMELA
# domain=/tools/pddl/ipc/IPC4/DOMAINS/PROMELA/OPTICAL_TELEGRAPH/STRIPS/P01_DOMAIN*.PDDL to P14_DOMAIN*.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/PROMELA/OPTICAL_TELEGRAPH/STRIPS/P01_OPT*.PDDL to P14_OPT*.PDDL
sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/PROMELA/OPTICAL_TELEGRAPH/STRIP" $runs "seq-sat" 1

# domain=/tools/pddl/ipc/IPC4/DOMAINS/PROMELA/PHILOSOPHERS/STRIPS/P01_DOMAIN*.PDDL to P29_DOMAIN*.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/PROMELA/PHILOSOPHERS/STRIPS/P01_PHIL*.PDDL to P29_PHIL*.PDDL
sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/PROMELA/PHILOSOPHERS/STRIP" $runs "seq-sat" 1

# PSR
# domain=/tools/pddl/ipc/IPC4/DOMAINS/PSR/SMALL/STRIPS/P01_DOMAIN.PDDL to P50_DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/PSR/SMALL/STRIPS/P01_*.PDDL to P50_*.PDDL
sge_submit_array_ipc "/tools/pddl/ipc/IPC4/DOMAINS/PSR/SMALL/STRIPS" $runs "seq-sat" 1

# SATELLITE
# domain=/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/STRIPS/STRIPS/DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/STRIPS/STRIPS/P01_*.PDDL to P36_*.PDDL
sge_submit_array_ipc_single_domain "/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/STRIPS/STRIPS" $runs "seq-sat" 1

# domain=/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/TIME/STRIPS_TEMPORAL/DOMAIN.PDDL
# instance=/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/TIME/STRIPS_TEMPORAL/P01_*.PDDL to P36_*.PDDL
sge_submit_array_ipc_single_domain "/tools/pddl/ipc/IPC4/DOMAINS/SATELLITE/TIME/STRIPS_TEMPORAL" $runs "tempo-sat" 0

