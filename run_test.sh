
  domain="./scripts/runs/ipc6_tempo-sat_elevators-strips_p01-domain.pddl"
instance="./scripts/runs/ipc6_tempo-sat_elevators-strips_p01.pddl"

res=$(mktemp --directory)

cmd="./debug/dae/dae --verbose=debug --max-seconds=1799 --domain=$domain --instance=$instance --seed=0 --status=$res/status/dae_status.${domain}_${instance}_s0 --plan-file=$res/plans/dae_plan_${domain}_${instance}_r0 --runs-max=1"

echo $cmd
$cmd

