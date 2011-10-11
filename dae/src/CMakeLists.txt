# include the sources

#INCLUDE_DIRECTORIES(${EO_SRC_DIR}/src)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# target the dae lib

SET (CMAKE_CXX_COMPILER mpicxx)

SET(DAEX_LIB_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
SET(LIBRARY_OUTPUT_PATH ${DAEX_LIB_OUTPUT_PATH})




SET(PARADISEO_DIR "/home/mustapha/framework/paradiseo-1.2.1" CACHE PATH "ParadisEO directory" FORCE)


#SET(PARADISEO_EO_SRC_DIR "${EO_DIR}" CACHE PATH "ParadisEO-EO source directory" FORCE)
#SET(PARADISEO_EO_BIN_DIR "${EO_DIR}/release" CACHE PATH "ParadisEO-EO binary directory" FORCE)

SET(PARADISEO_EO_SRC_DIR "${PARADISEO_DIR}/paradiseo-eo" CACHE PATH "ParadisEO-EO source directory" FORCE)
SET(PARADISEO_EO_BIN_DIR "${PARADISEO_DIR}/paradiseo-eo/build" CACHE PATH "ParadisEO-EO binary directory" FORCE)



#SET(EO_DIR "/home/mustapha/framework/dae/eo" CACHE PATH "EO directory" FORCE)
#SET(EO_BIN_DIR "${EO_DIR}/release" CACHE PATH "EO binary directory" FORCE)
#SET(EO_SRC_DIR "${EO_DIR}/src" CACHE PATH "EO source directory" FORCE)



SET (DAEX_SOURCES
  utils/posix_timer.cpp
  utils/pddl_load.cpp
  utils/evalBestPlanDump.cpp
  core/decomposition.cpp
  core/goal.cpp
  initialization/init.cpp
  evaluation/cpt-yahsp.cpp
  evaluation/yahsp.cpp
  variation/addgoal.cpp
  variation/delgoal.cpp
  variation/delatom.cpp
  pack/daePackUnpack.cpp ) 
#  pack/eoDualFitness.h)


#SET (DAEX_PACK 
# pack/daePackUnpack.cpp
 # utils/posix_timer.cpp
 # utils/pddl_load.cpp
 # utils/evalBestPlanDump.cpp
 # core/decomposition.cpp
 # core/goal.cpp
 # initialization/init.cpp
 # evaluation/cpt-yahsp.cpp
 # evaluation/yahsp.cpp
 # variation/addgoal.cpp
 # variation/delgoal.cpp
 # variation/delatom.cpp)



INCLUDE_DIRECTORIES(${PARADISEO_EO_SRC_DIR}/src 
                    ${PARADISEO_MO_SRC_DIR}/src
                    ${PARADISEO_MOEO_SRC_DIR}/src
                    ${PARADISEO_PEO_SRC_DIR}/src
		    ${DAEX_SRC_DIR}/src
		    ${DAEX_SRC_DIR}/src/core
 		    ${DAEX_SRC_DIR}/src/utils
		    ${DAEX_SRC_DIR}/src/pack
	  	    ${DAEX_SRC_DIR}/initialization
    		    ${DAEX_SRC_DIR}/evaluation
 		    ${DAEX_SRC_DIR}/variation		    
		    ${DAE_SRC_DIR})
#		    ${EO_SRC_DIR}/src
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


ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(evaluation)
ADD_SUBDIRECTORY(initialization)
ADD_SUBDIRECTORY(pack)
ADD_SUBDIRECTORY(utils)
ADD_SUBDIRECTORY(variation)

