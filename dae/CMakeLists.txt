CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# include the install configuration file where are defined the main variables
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/install.cmake OPTIONAL)


SET(EO_SRC_DIR "${PARADISEO_DIR}/eo/src" CACHE PATH "EO source directory" FORCE)
SET(MOEO_SRC_DIR "${PARADISEO_DIR}/moeo/src" CACHE PATH "EO source directory" FORCE)
SET(MO_SRC_DIR "${PARADISEO_DIR}/mo/src" CACHE PATH "EO source directory" FORCE)

SET(EO_BIN_DIR "${PARADISEO_BUILD_DIR}/eo/lib" CACHE PATH "EO binary directory" FORCE)
SET(MOEO_BIN_DIR "${PARADISEO_BUILD_DIR}/moeo/lib" CACHE PATH "EO binary directory" FORCE)
SET(MO_BIN_DIR "${PARADISEO_BUILD_DIR}/mo/lib" CACHE PATH "EO binary directory" FORCE)

# the source directory of yahsp is not /src/ because we include src/cpt.h instead of cpt.h
# because of name collisions
SET(CPT_YAHSP_SRC_DIR "${CPT_YAHSP_DIR}" CACHE PATH "CPT-YAHSP source directory" FORCE)
SET(CPT_YAHSP_BIN_DIR "${CPT_YAHSP_BUILD_DIR}/lib" CACHE PATH "CPT-YAHSP source directory" FORCE)

SET(DAE_MO "false" CACHE BOOL "Build the Multi-Objective version of DaE")
IF( DAE_MO )
    SET( BINARY_NAME "daemo" CACHE STRING "Name of the built binary")
    ADD_DEFINITIONS(-DDAE_MO)
ELSE()
    SET( BINARY_NAME "dae" CACHE STRING "Name of the built binary")
ENDIF()

# define project name
PROJECT(DAEX)

# include useful features for cmake
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

IF( DAE_WITH_OMP )
    FIND_PACKAGE(OpenMP REQUIRED)
    IF(OPENMP_FOUND)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    ENDIF()

    ADD_DEFINITIONS(-DWITH_OMP)
ENDIF()

# Mpi Addons
IF(DAE_WITH_MPI)
    MESSAGE("[DAE] Compilation with MPI.")

    SET(CMAKE_CXX_COMPILER "${MPI_DIR}/bin/mpicxx")

    # headers location
    INCLUDE_DIRECTORIES(${MPI_DIR}/include)

    # lib location
    LINK_DIRECTORIES(${MPI_DIR}/lib)

    # for conditional compilation in code
    ADD_DEFINITIONS(-DWITH_MPI)

    LINK_LIBRARIES(eompi)
ENDIF()

# include the sources
INCLUDE_DIRECTORIES(
  ${EO_SRC_DIR}
  ${MOEO_SRC_DIR}
  ${MO_SRC_DIR}
  ${CPT_YAHSP_SRC_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

# where CMake can find the libraries
LINK_DIRECTORIES(
  ${EO_BIN_DIR}
  ${MOEO_BIN_DIR}
  ${MO_BIN_DIR}
  ${CPT_YAHSP_BIN_DIR}
  )

# flags to use with compiler
# VV : -DDAE for YAHSP headers, -D__STDC_CONSTANT_MACROS for macros INT64_C...
ADD_DEFINITIONS(-W -Wall -DDAE -D__STDC_CONSTANT_MACROS)

IF (NOT (CMAKE_BUILD_TYPE MATCHES Debug))
 ADD_DEFINITIONS(-O3 -DNDEBUG)
ENDIF()
#  #ADD_DEFINITIONS(-DPAPERVERSION)
#  ADD_DEFINITIONS(-pg -g)
#ELSE()
 
# target + link libraries
LINK_LIBRARIES(eoutils gmp eo eoserial rt gcov gdsl termcap)

# VV  : cpt_yahsp_library-common-dae-omp and  cpt_yahsp_library-dae are  no more
# factorized because dae + yahsp-mt need  to be completely recompiled due to TLS
# global references that differ from the ones needed by dae-mt + yahsp.

ADD_EXECUTABLE(${BINARY_NAME} dae.cpp)
ADD_DEPENDENCIES(${BINARY_NAME} daex)
IF( CPT_WITH_OMP )
    TARGET_LINK_LIBRARIES(${BINARY_NAME}   daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae)
ELSE()
    TARGET_LINK_LIBRARIES(${BINARY_NAME}   daex cpt_yahsp_library-common-dae     cpt_yahsp_library-dae)
ENDIF()

# only if OpenMP on yahsp is activated
IF (CPT_WITH_OMP AND DAE_WITH_OMP )
    ADD_EXECUTABLE(${BINARY_NAME}-sm dae.cpp)
    ADD_DEPENDENCIES(${BINARY_NAME}-sm daex)
    TARGET_LINK_LIBRARIES(dae-sm daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae-sm)

    ADD_EXECUTABLE(${BINARY_NAME}-mt dae.cpp)
    ADD_DEPENDENCIES(${BINARY_NAME}-mt daex-yahsp-mt)
    TARGET_LINK_LIBRARIES(${BINARY_NAME}-mt daex-yahsp-mt cpt_yahsp_library-dae-mt)
    SET_TARGET_PROPERTIES(
    ${BINARY_NAME}-mt
    PROPERTIES 
    COMPILE_FLAGS "-DYAHSP_MT"
    )
ENDIF()

IF (CMAKE_BUILD_TYPE MATCHES Debug)
    # Debug + OMP
    IF( CPT_WITH_OMP )
        ADD_EXECUTABLE(featureselector featureselector.cpp)
        ADD_DEPENDENCIES(featureselector daex)
        TARGET_LINK_LIBRARIES(featureselector daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae)
    ELSE()
    # Debug without OMP
        ADD_EXECUTABLE(featureselector featureselector.cpp)
        ADD_DEPENDENCIES(featureselector daex)
        TARGET_LINK_LIBRARIES(featureselector daex cpt_yahsp_library-common-dae cpt_yahsp_library-dae)
    ENDIF()

    ADD_SUBDIRECTORY(tests)
ENDIF()

# process to sources
ADD_SUBDIRECTORY(src)

