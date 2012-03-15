# include the sources

#INCLUDE_DIRECTORIES(${EO_SRC_DIR}/src)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# target the dae lib

SET (CMAKE_CXX_COMPILER mpicxx)

SET(DAEX_LIB_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
SET(LIBRARY_OUTPUT_PATH ${DAEX_LIB_OUTPUT_PATH})




SET(PARADISEO_DIR "<>" CACHE PATH "ParadisEO directory" FORCE)
 

SET(PARADISEO_EO_SRC_DIR "${PARADISEO_DIR}/paradiseo-eo" CACHE PATH "ParadisEO-EO source directory" FORCE)
SET(PARADISEO_EO_BIN_DIR "${PARADISEO_DIR}/paradiseo-eo/build" CACHE PATH "ParadisEO-EO binary directory" FORCE)

 


SET (DAEX_SOURCES
  utils/posix_timer.cpp
  utils/pddl_load.cpp
  #utils/evalBestPlanDump.cpp
  core/decomposition.cpp
  core/goal.cpp
  initialization/init.h
  evaluation/cpt-yahsp.h
  evaluation/yahsp.h
  variation/addgoal.h
  variation/delgoal.h
  variation/delatom.h)

INCLUDE_DIRECTORIES(${PARADISEO_EO_SRC_DIR}/src 
                    ${PARADISEO_MO_SRC_DIR}/src
                    ${PARADISEO_MOEO_SRC_DIR}/src
                    ${PARADISEO_PEO_SRC_DIR}/src
		    ${DAEX_SRC_DIR}/src
		    ${DAEX_SRC_DIR}/src/core
 		    ${DAEX_SRC_DIR}/src/utils
		    ${DAEX_SRC_DIR}/src/pack
	  	    ${DAEX_SRC_DIR}/src/initialization
    		    ${DAEX_SRC_DIR}/src/evaluation
 		    ${DAEX_SRC_DIR}/src/variation
		    ${DAE_SRC_DIR}/do		    
		    ${DAE_SRC_DIR})
#		 
######################################################################################


######################################################################################
###  Specify where CMake can find the libraries
######################################################################################

# Link with the EO library directory, only if you need it
LINK_DIRECTORIES(${PARADISEO_EO_BIN_DIR}/lib)

# Link with the MOEO library directory, only if you need it
LINK_DIRECTORIES(${PARADISEO_MOEO_BIN_DIR}/lib)

# Link with the PEO library directory, only if you need it
LINK_DIRECTORIES(${PARADISEO_PEO_BIN_DIR}/lib)

LINK_DIRECTORIES(${PARADISEO_DIR}/libxml2/lib)

LINK_DIRECTORIES(${DAEX_BIN_DIR}/lib
		 #${EO_BIN_DIR}/lib
		${DAE_BIN_DIR})
 
ADD_LIBRARY(daex STATIC ${DAEX_SOURCES})

ADD_LIBRARY(daex-yahsp-mt STATIC ${DAEX_SOURCES})


TARGET_LINK_LIBRARIES( daex xml2 peo rmc_mpi eo eoutils )
SET_TARGET_PROPERTIES(
  daex-yahsp-mt
  PROPERTIES 
  COMPILE_FLAGS "-DYAHSP_MT"
  )

 
