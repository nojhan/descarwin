
#ifndef DAEPACKUNPACK_H_
#define DAEPACKUNPACK_H_		
#include <eo>	
#include <peo>
#include <core/eoVector_mesg.h>
#include <core/messaging.h>
#include "daex.h"
#include <iostream>
#include <utility>
#include <string>
//#include "eoDualFitness.h"
//#include "core/decomposition.h"
 //#include "core/goal.h"
// #include "core/plan.h"
// #include "core/atom.h"
// #include "utils/pddl.h"
// #include "utils/pddl_load.h"
// #include "evaluation/yahsp.h"
// #include "utils/evalBestPlanDump.h"
// #include "evaluation/cpt-yahsp.h"
// #include "src/globs.h"
// #include "src/yahsp.h"
// #include <gmpxx.h>
// //extern "C" {
// #include <src/cpt.h>
// #include <src/branching.h>
// #include <src/structs.h>
// #include <src/problem.h>
// #include <src/globs.h>
// #include <gmpxx.h>
// #include <src/plan.h>
// #include <src/options.h>
// #include <src/solve.h>
//}


//#define  eoMinimizingDualFitness  eoDualFitness<double, std::greater<double>> 



using namespace daex; 



 //extern   std::vector< daex::Atom*>  globalAtoms; 
 
	void pack(   daex::Decomposition  &  _eo );
	
	void unpack (daex::Decomposition & _eo);

	void pack(   daex::Plan  &  _plan_global );

	void unpack(  daex::Plan & _plan_global );

	void pack(   std::vector< daex::Plan > &  _plans_sub);

	void unpack(std::vector< daex::Plan >& _plans_sub);

	void pack(daex::Goal &  _goal);

	void unpack(daex::Goal &_goal);

	void pack(daex::Atom  & _atom);

	void unpack(daex::Atom & _atom);
 

	template <class BaseType, class Compare >

	void pack (const eoDualFitness <BaseType, Compare> &  fit)  {   //eoMinimizingDualFitness

		pack(fit.value());

		pack(fit.is_feasible());

	};

	template <class BaseType, class Compare >
	
	void unpack (eoDualFitness<BaseType, Compare> &  fito) {

		BaseType value;

		bool is_feasible;

		unpack(value);

		unpack(is_feasible);
	
		fito.setValue(value);

		fito.setFeasible(is_feasible);

	}	


	template <class BaseType>
	
	void pack (  const  std::pair<BaseType,bool>& v) {

		 
		pack(v.first);

		pack(v.second);


	}



	template <class BaseType>
	
	void unpack (std::pair<BaseType,bool>& v) {

			
		unpack(v.first);

		unpack(v.second);


	} 

		
/*
	void pack ( const eoMinimizingDualFitness &   fit) ;

	void unpack (eoMinimizingDualFitness &  fit) ; 

 	void pack ( std::pair<double,bool> & v) ;

	void unpack (std::pair<double,bool>& v) ;*/
			 

/*
void pack( std::pair<double,bool>&   fit);
	

	void unpack(std::pair<double,bool>&   & fit);*/




#endif /*DAEPACKUNPACK_H_*/
