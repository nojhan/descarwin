CPTSolutionPlan cptSolveNext2(CPTFluentArray goal)
{
  start_timer(search_time);
  return_code = cpt_search(state, state_nb, goal.elems, goal.nb, false, false, false);
  
  if (solution_plan) {
  	return_code = cpt_search(init_state, init_state_nb, goal.elems, goal.nb, true, false,false);
  	if (solution_plan) {
    	solution_plan->search_time = get_timer(search_time);
    	solution_plan->total_time = solution_plan->search_time;
    	plans[plans_nb++] = solution_plan;
    	total_actions += solution_plan->steps_nb;
    	apply_plan();
    	solution_plan = NULL;
    	return plans[plans_nb - 1];
  	}
  	return NULL;
  } else return NULL;
}

void cptPrintSolutionPlan2(FILE *out, CPTSolutionPlan solution_plan,double precision)
{
	CPTprint_plan(out, solution_plan, false, precision);
}
