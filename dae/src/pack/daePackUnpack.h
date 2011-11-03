
#ifndef DAEPACKUNPACK_H_
#define DAEPACKUNPACK_H_		
#include <eo>	
#include <peo>
//#include <core/eoVector_mesg.h> attention
//#include <core/messaging.h>
//#include <pack/eoVector_mesg.h>
#include <pack/daeMessaging.h>
//#include "packUnpackeoScalarFitness.h"
#include "daex.h"
#include <iostream>
#include <utility>
#include <string>



 #define  eoMinimizingDualFitness  eoDualFitness< double, std::greater<double> > 



using namespace daex; 



 
 
	void pack( daex::Decomposition  &  _eo );
	
	void unpack (daex::Decomposition & _eo);

	void pack( daex::Plan  &  _plan_global );

	void unpack(daex::Plan & _plan_global );

	void pack(std::vector< daex::Plan > &  _plans_sub);

	void unpack(std::vector< daex::Plan >& _plans_sub);

	void pack(daex::Goal &  _goal);

	void unpack(daex::Goal &_goal);

	void pack(daex::Atom  & _atom);

	void unpack(daex::Atom & _atom);
	
	
	void pack ( const   std::pair<double,bool> & v) ;
	
	void unpack (std::pair<double,bool> & v) ;
 

	//void pack ( const eoDualFitness<double, std::greater<double> > &   fit) ;

//	void unpack (eoDualFitness<double, std::greater<double> >  &  fit) ; 

	
	template <class BaseType, class Compare >

	void pack (const eoDualFitness <BaseType, Compare> &  fit)  {   

	 	 
		pack(fit.value());
 
		pack(fit.is_feasible());
 

	}; 

	 template <class BaseType, class Compare >
	
	void unpack ( eoDualFitness<BaseType, Compare> &  fit) {

	 
		BaseType value;				

		bool is_feasible;

		unpack(value);

		unpack(is_feasible);
		
		fit.setValue(value);

		fit.setFeasible(is_feasible);
	 

	}; 

/*void pack (const eoMinimizingDualFitness  & fit );
	
	void unpack (eoMinimizingDualFitness  & fit );*/
	
// 	template <class BaseType>
// 	
// 	void pack (  const  std::pair<BaseType,bool>& v) {
// 
// 		 
// 		pack(v.first);
// 
// 		pack(v.second);
// 
// 
// 	}
// 
// 
// 
// 	template <class BaseType>
// 	
// 	void unpack (std::pair<BaseType,bool>& v) {
// 
// 			
// 		unpack(v.first);
// 
// 		unpack(v.second);
// 
// 
// 	} 

		
 

 	/*void pack ( std::pair<double,bool> & v) ;

	void unpack (std::pair<double,bool>& v) ;*/
			 

/*
void pack( std::pair<double,bool>&   fit);
	

	void unpack(std::pair<double,bool>&   & fit);*/




#endif /*DAEPACKUNPACK_H_*/
