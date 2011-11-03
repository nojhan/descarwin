	
#include "daePackUnpack.h"

using namespace daex; 

 extern std::vector< daex::Atom*>  daex::globalAtoms; 
 	
	void pack( daex::Decomposition  & _eo)
		{	
		
				
		  if ( _eo.invalid() ) 
    		  	pack( (unsigned int)0 );
		    else
		      {
		      	pack( (unsigned int)1 );
  
 		      pack(_eo.fitness().value());
 		       
 		      pack(_eo.fitness().is_feasible());
 
         	   //  pack( _eo.fitness());
// 		       
		     //  std::cout <<"pack"<<_eo.fitness().value()<<_eo.fitness().is_feasible()<<std::endl;
			
		     }
		 
   		unsigned len = _eo.size();

		pack(len); 
 
  		std::list<Goal>::iterator it;

		for ( it=_eo.begin() ; it != _eo.end(); it++ ){
		    
    			pack (*it); ///pack each goal (liste de pointeur d'atomes)
    				
    		    		
    		}
    		
    		// std::cout<<"packeo"<<_eo<<std::endl;
 
		pack(_eo. plan());
 	
		pack(_eo.subplans());
 
		pack(_eo. b_max());
				 
		pack(_eo.get_number_evaluated_goals());
		 
		 		
		pack(_eo.get_number_useful_goals());
		
		 
		pack(_eo.get_number_evaluated_nodes());
 
	 }

	void unpack (daex::Decomposition  & _eo)
	{
						
		unsigned valid, len, _b_max, _k, _u, _B; 
		
 
		 unpack(valid);
		
  		 if (! valid) 
 
      		 	 _eo.invalidate();
 
  		 else
    		 {
    			 eoDualFitness<double, std::greater<double> > fit; 
     			 
    		//	 unpack(fit);
 
  			double value ;
  			 
 			bool is_feasible;
  					
 			unpack(value);
   			
   			unpack(is_feasible);
  			 
   			 fit.setValue(value);
 		 
 			 fit.setFeasible(is_feasible);
 
		 	 _eo.fitness (fit);
			 
			// std::cout <<"unpack"<<_eo.fitness().value()<<_eo.fitness().is_feasible()<<std::endl;
			
		 }

       		unpack (len);
  		
  		_eo.clear();
  		
  		 daex::Goal _goal(0);
  		                       
		for (unsigned i =0; i < len; i++){

			unpack(_goal); ///attention liste de pointeurs
				
			_eo.push_back(_goal);
		}
		
		// std::cout<<"unpackeo"<<_eo<<std::endl;
		
// 		std::list<Goal>::iterator it;
// 		
// 		for ( it=_eo.begin() ; it != _eo.end(); it++ ){
// 		
// 		  unpack(_goal);
// 		
// 		 *it= _goal ;
// 		

		Plan _plan_global ;		
		
		unpack(_plan_global);
		
		_eo.plan_global(_plan_global); 
		
	 	//std::cout<<_eo.plan()<<std::endl;
						
		std::vector< Plan > _plans_sub  ;
		           
		unpack(_plans_sub); 
		
		_eo.subplans()=_plans_sub;
		
// 		for (size_t i= 0; i< _plans_sub.size(); i++)
// 		
// 			std::cout<<"sub_plan"<<_plans_sub[i]<<std::endl;
// 		
//  	for (size_t i= 0; i< _eo.subplans_size(); i++)
//  		
//  			std::cout<<"sub_plan"<<_eo.subplans()[i]<<std::endl;

		unpack(_b_max);
		
		_eo.b_max(_b_max);

		unpack(_k);

		_eo.reset_number_evaluated_goals();
		
		_eo.incr_number_evaluated_goals(_k);
		
		unpack(_u);

		_eo.reset_number_useful_goals();

		_eo.incr_number_useful_goals(_u);

		unpack(_B);

		_eo.reset_number_evaluated_nodes();

		_eo.incr_number_evaluated_nodes(_B);	
		
		
	}



	void pack(    daex::Plan   &  _plan_global )
	{

		pack( _plan_global.makespan());
		
		pack(_plan_global.search_steps());

		pack(_plan_global.isValid());///il falait un accesseur sur _is_valid

   	        unsigned len = _plan_global.plan_rep().size();
   	        
   	        pack(len);
 
 		for ( unsigned i =0; i < len ; i++)
 		
 			pack(_plan_global.plan_rep()[i]);

			
	}

	void unpack(  daex::Plan &  _plan_global )
	{

		TimeVal _makespan;

		unsigned _search_steps;

		bool _is_valid;

		unpack(_makespan); ///mutateur sur makespan
		
		_plan_global.makespan(_makespan);

		unpack(_search_steps);

		_plan_global.search_steps(_search_steps);

		unpack(_is_valid);
		
		_plan_global.isValid(_is_valid);

		unsigned len;

		unpack(len);
		
		_plan_global.plan_rep().clear();

		_plan_global.plan_rep().resize(len);

		char ch;

		for ( unsigned i =0; i < len ; i++){
		
			unpack(ch);
			
			_plan_global.plan_rep()[i]= ch;
		}
			
	}


	void pack(    std::vector< Plan >  & _plans_sub)
	{
		unsigned len = _plans_sub.size();
		
		pack(len);

		for(size_t i =0; i<len; i++)

			pack(_plans_sub[i]);
			

	}
	
	void unpack(std::vector< Plan >&  _plans_sub)
	{

		unsigned len;

		unpack(len);
		
		_plans_sub.clear();

		_plans_sub.resize(len);
		
		

		for( size_t i =0; i < len ; i++)

			unpack(_plans_sub[i]);
			
	
	}

	void pack(    daex::Goal  & _goal){

		pack(_goal.earliest_start_time());
		
		unsigned len = _goal.size();

		pack(len);

		std::list<Atom*>::iterator it;   

		for ( it=_goal.begin() ; it != _goal.end(); it++ ) {
		    
    			pack( (*it) ->  fluentIndex());  
    			
    			//pack(*(*it));
    			
    			//std::cout << "pack "<<(*(*it))<<std::endl;
    			
    			}
 		


	}


	void unpack(daex::Goal &_goal){ 
		 
		TimeVal _ptime; 

		unpack(_ptime);

		_goal.earliest_start_time(_ptime);
  		 			
		unsigned len;

		unpack(len);

		_goal.clear();
				
		unsigned index_atom;
		
		for ( size_t i = 0; i < len ; i++) {

			unpack(index_atom);
			
			_goal.push_back(daex::globalAtoms[index_atom]); 
 			
			}						
			
	}
			
			
		 //_goal.resize(len);	
 
		 //Atom  _atom(0,0); 
		
		 //std::list<Atom*>::iterator it;   
		 
		 //for ( it=_goal.begin() ; it != _goal.end(); it++ ) {
		
 
 			// unpack (_atom);
 	
		//	*it =  new Atom (_atom.earliest_start_time(),_atom.fluentIndex());
					//_goal.push_back(new 
		//Atom(_atom.earliest_start_time(),_atom.fluentIndex())) ///other possibility

			 
		//}	 

		

	void pack(    daex::Atom  & _atom){

		pack(_atom.earliest_start_time());

		pack(_atom.fluentIndex());  
		
		
	}


	void unpack(daex::Atom & _atom){

		
		TimeVal  _earliest_start_time;

		unsigned _fluent;

		unpack(_earliest_start_time);

		_atom.earliest_start_time (_earliest_start_time);

		unpack(_fluent);

		_atom.fluentIndex(_fluent);
	
	

	}


/*
	void pack (  const eoMinimizingDualFitness  &  fit)  {    

		pack(fit.value());

		pack(fit.is_feasible());

	}




	void unpack (eoMinimizingDualFitness &  fit) {

		double value;

		bool is_feasible;

		unpack(value);

		unpack(is_feasible);
	
		fit.setValue(value);

		fit.setFeasible(is_feasible);

	}	


 */
	
	void pack ( const   std::pair<double,bool> & v) {

		 
 		pack(v.first);
 
 		pack(v.second);


	}


 
	void unpack (std::pair<double,bool>& v) {

			
		unpack(v.first);

		unpack(v.second);


	}
/*
	void pack( const  eoDualFitness<double, std::greater<double> >  &  fit)  {

		pack(fit.value()); 
		
		 std::cout<<"packfit"<<fit.value()<<std::endl;
		

		pack(fit.is_feasible());
		
		 std::cout<<"packfit"<<fit.is_feasible()<<std::endl;

	}

	void unpack(eoDualFitness<double, std::greater<double> >  & fit) { 
 
		double value ;

		bool is_feasible;

		unpack(value);
		
		unpack(is_feasible);
			 		
		fit.setValue(value);
		
		//std::cout<<"packfit"<<fit.value()<<std::endl;
 
 		fit.setFeasible(is_feasible);
 		
 		 //std::cout<<"packfit"<<fit.is_feasible()<<std::endl;
}*/
 
