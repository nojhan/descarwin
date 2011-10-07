#CMAKE DAE
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# include the install configuration file where are defined the main variables
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/install.cmake OPTIONAL)

# define project name
PROJECT(DAEPEO)

# include useful features for cmake
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules) 

#IF (WITH_OPENMP)
 FIND_PACKAGE(OpenMP REQUIRED)

 IF(OPENMP_FOUND)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
   SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
 ENDIF()
#ENDIF()




ADD_CUSTOM_TARGET(install DEPENDS ${DAEPEO_SOURCE_DIR}/param ${DAEPEO_SOURCE_DIR}/schema.xml)

ADD_CUSTOM_COMMAND(
    TARGET install
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_if_different
      ${DAEPEO_SOURCE_DIR}/param
      ${DAEPEO_BINARY_DIR})
ADD_CUSTOM_COMMAND(
    TARGET install
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_if_different
      ${DAEPEO_SOURCE_DIR}/schema.xml
      ${DAEPEO_BINARY_DIR} )





# include the sources
INCLUDE_DIRECTORIES(
 # ${EO_SRC_DIR}
  ${DAEPEO_SOURCE_DIR}/src
  ${DAEPEO_SOURCE_DIR}/src_cpt
  ${PARADISEO_EO_SRC_DIR}/src 
  ${PARADISEO_MO_SRC_DIR}/src  
  ${PARADISEO_MOEO_SRC_DIR}/src 
  ${PARADISEO_PEO_SRC_DIR}/src
  ${CPT_YAHSP_SRC_DIR}
  ${GDSL_LIBRARY}
  ${CMAKE_CURRENT_SOURCE_DIR}/src

		    
  )




# where CMake can find the libraries
LINK_DIRECTORIES(
  #${EO_BIN_DIR}/lib
  ${GDSL_LIBRARY}
  ${CPT_YAHSP_DIR}/lib
  ${DAEPEO_BINARY_DIR}/lib}

  )

# Link with the EO library directory, only if you need it
LINK_DIRECTORIES(${PARADISEO_EO_BIN_DIR}/lib)

LINK_DIRECTORIES(${PARADISEO_MO_BIN_DIR}/lib)

# Link with the MOEO library directory, only if you need it
LINK_DIRECTORIES(${PARADISEO_MOEO_BIN_DIR}/lib)

# Link with the PEO library directory, only if you need it
LINK_DIRECTORIES(${PARADISEO_PEO_BIN_DIR}/lib)


# flags to use with compiler
# VV : -DDAE pour les headers de YAHSP
ADD_DEFINITIONS(-W -Wall -DDAE) 

IF (CMAKE_BUILD_TYPE MATCHES Debug)
  #ADD_DEFINITIONS(-DPAPERVERSION)
  ADD_DEFINITIONS(-pg -g)
ELSE()
  ADD_DEFINITIONS(-O3 -DNDEBUG)
ENDIF()

# target + link libraries
LINK_LIBRARIES(eoutils gmp eo gcov gdsl)

# VV  : cpt_yahsp_library-common-dae-omp and  cpt_yahsp_library-dae are  no more
# factorized because dae + yahsp-mt need  to be completely recompiled due to TLS
# global references that differ from the ones needed by dae-mt + yahsp.

ADD_EXECUTABLE(dae dae.cpp)
ADD_DEPENDENCIES(dae daex)
TARGET_LINK_LIBRARIES(dae daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae)


######################################################################################
### 4) Define your target: just an executable here
######################################################################################
#ADD_EXECUTABLE(daePeo daePeo.cpp) 
#ADD_DEPENDENCIES(daePeo daex peo rmc_mpi)



#TARGET_LINK_LIBRARIES(daePeo daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae 
#			  ${XML2_LIBS} peo rmc_mpi  eo  ga es eoutils peo)



#ADD_EXECUTABLE(dae-sm dae.cpp)
#ADD_DEPENDENCIES(dae-sm daex)
#TARGET_LINK_LIBRARIES(dae-sm daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae-sm)

#ADD_EXECUTABLE(dae-mt dae.cpp)
#ADD_DEPENDENCIES(dae-mt daex-yahsp-mt)
#TARGET_LINK_LIBRARIES(dae-mt daex-yahsp-mt cpt_yahsp_library-dae-mt)

#ADD_EXECUTABLE(test_chronopartition test_chronopartition.cpp)
#ADD_DEPENDENCIES(test_chronopartition daex)
#TARGET_LINK_LIBRARIES(test_chronopartition daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae)

#ADD_EXECUTABLE(test_b_max test_b_max.cpp)
#ADD_DEPENDENCIES(test_b_max daex)
#TARGET_LINK_LIBRARIES(test_b_max daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae)

#ADD_EXECUTABLE(test_stats test_stats.cpp)
#ADD_DEPENDENCIES(test_stats daex)
#TARGET_LINK_LIBRARIES(test_stats daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae)

#ADD_EXECUTABLE(featureselector featureselector.cpp)
#ADD_DEPENDENCIES(featureselector daex)
#TARGET_LINK_LIBRARIES(featureselector daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae)



#PEO Part



# process to sources
ADD_SUBDIRECTORY(src)
