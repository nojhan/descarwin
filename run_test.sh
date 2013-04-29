
cd builds/

build_dir=$1 # directory where DAE has been built (debug, release or whatever)


if [[ "$build_dir" = "daemo"* ]]; then
    bin="./${build_dir}/dae/daemo"
      domain="../instances/multi-objectives/zenoMiniMulti/domain-zeno-time-miniRisk.pddl"
    instance="../instances/multi-objectives/zenoMiniMulti/zeno3eMiniMulti.pddl"
    specific=""
else
    bin="./${build_dir}/dae/dae"
      domain="../scripts/runs/ipc6_tempo-sat_elevators-strips_p01-domain.pddl"
    instance="../scripts/runs/ipc6_tempo-sat_elevators-strips_p01.pddl"
    specific=""
fi

d=$(basename $domain .pddl)
i=$(basename $instance .pddl)
plan="./${bin}_test_${d}_${i}_s0.plan"

common="--domain=${domain} --instance=${instance} --seed=0 --verbose=debug
--status=./${bin}_test_${d}_${i}_s0.status --plan-file=${plan}
--max-seconds=1799 --runs-max=1 --gen-max=3 --bmax-fixed=100"

cmd="${bin} ${common} ${specific}"

echo $cmd
$cmd

# echo "PLAN:"
# cat $plan

