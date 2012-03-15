/*
* <dae_genotype_dae.h>
* Copyright (C) TAO Project-Team, INRIA Saclay-LRI && Thales Group, 2011-2012
* DESCARWIN ANR project 
* Author: <Mostepha R. Khouadjia>
* Copyright: See COPYING file that comes with this distribution
*
*
*/

#ifndef MAKE_GENOTYPE_DAE_H_
#define MAKE_GENOTYPE_DAE_H_


#include <utils/eoParser.h>
#include <utils/eoState.h>
#include <daex.h>


 
 daex::Init<daex::Decomposition> & do_make_genotype(eoParser& _parser, eoState& _state, daex::pddlLoad & _pddl)
{
 	
	  unsigned int l_max_init_coef = _parser.createParam( (unsigned int)2, "lmax-initcoef", "l_max will be set to the size of the chrono partition * this coefficient", 'C', "Initialization" ).value();
            
          unsigned int l_min = _parser.createParam( (unsigned int)1, "lmin", 
          "Minimum number of goals in a decomposition", 'M', "Initialization").value();  
          
          
          
          daex::Init<daex::Decomposition> *init = new daex::Init<daex::Decomposition> ( _pddl.chronoPartitionAtom(), l_max_init_coef, l_min );
    	
    	_state.storeFunctor(init);
    	
    	
    	return *init;
}
    	
    	
#endif  /* MAKE_GENOTYPE_DAE_H_*/
