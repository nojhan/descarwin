
  domain="./scripts/runs/ipc6_tempo-sat_elevators-strips_p01-domain.pddl"
instance="./scripts/runs/ipc6_tempo-sat_elevators-strips_p01.pddl"

d=$(basename $domain .pddl)
i=$(basename $instance .pddl)
plan="./dae_test_${d}_${i}_s0.plan"

cmd="./debug/dae/dae --domain=${domain} --instance=${instance} --seed=0 --verbose=debug
--status=./dae_test_${d}_${i}_s0.status --plan-file=${plan}
--max-seconds=1799 --runs-max=1 --gen-max=3 --bmax-fixed=100 $*"

echo $cmd
$cmd

echo "PLAN:"
cat $plan

