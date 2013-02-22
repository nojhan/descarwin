CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# include the install configuration file where are defined the main variables
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/install.cmake OPTIONAL)

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
  ${CPT_YAHSP_SRC_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

# where CMake can find the libraries
LINK_DIRECTORIES(
  ${EO_BIN_DIR}/lib
  ${CPT_YAHSP_BIN_DIR}/lib
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
LINK_LIBRARIES(eoutils gmp eo rt gcov gdsl termcap)

# VV  : cpt_yahsp_library-common-dae-omp and  cpt_yahsp_library-dae are  no more
# factorized because dae + yahsp-mt need  to be completely recompiled due to TLS
# global references that differ from the ones needed by dae-mt + yahsp.

ADD_EXECUTABLE(dae dae.cpp)
ADD_DEPENDENCIES(dae daex)
IF( CPT_WITH_OMP )
    TARGET_LINK_LIBRARIES(dae daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae)
ELSE()
    TARGET_LINK_LIBRARIES(dae daex cpt_yahsp_library-common-dae     cpt_yahsp_library-dae)
ENDIF()

# only if OpenMP on yahsp is activated
IF (CPT_WITH_OMP AND DAE_WITH_OMP )
    ADD_EXECUTABLE(dae-sm dae.cpp)
    ADD_DEPENDENCIES(dae-sm daex)
    TARGET_LINK_LIBRARIES(dae-sm daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae-sm)

    ADD_EXECUTABLE(dae-mt dae.cpp)
    ADD_DEPENDENCIES(dae-mt daex-yahsp-mt)
    TARGET_LINK_LIBRARIES(dae-mt daex-yahsp-mt cpt_yahsp_library-dae-mt)
    SET_TARGET_PROPERTIES(
    dae-mt
    PROPERTIES 
    COMPILE_FLAGS "-DYAHSP_MT"
    )
ENDIF()

IF (CMAKE_BUILD_TYPE MATCHES Debug)
    # Debug + OMP
    IF( CPT_WITH_OMP )
        ADD_EXECUTABLE(test_chronopartition test_chronopartition.cpp)
        ADD_DEPENDENCIES(test_chronopartition daex)
        TARGET_LINK_LIBRARIES(test_chronopartition daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae)

        ADD_EXECUTABLE(test_b_max test_b_max.cpp)
        ADD_DEPENDENCIES(test_b_max daex)
        TARGET_LINK_LIBRARIES(test_b_max daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae)

        ADD_EXECUTABLE(test_stats test_stats.cpp)
        ADD_DEPENDENCIES(test_stats daex)
        TARGET_LINK_LIBRARIES(test_stats daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae)

        ADD_EXECUTABLE(featureselector featureselector.cpp)
        ADD_DEPENDENCIES(featureselector daex)
        TARGET_LINK_LIBRARIES(featureselector daex cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae)
    ELSE()
    # Debug without OMP
        ADD_EXECUTABLE(test_chronopartition test_chronopartition.cpp)
        ADD_DEPENDENCIES(test_chronopartition daex)
        TARGET_LINK_LIBRARIES(test_chronopartition daex cpt_yahsp_library-common-dae cpt_yahsp_library-dae)

        ADD_EXECUTABLE(test_b_max test_b_max.cpp)
        ADD_DEPENDENCIES(test_b_max daex)
        TARGET_LINK_LIBRARIES(test_b_max daex cpt_yahsp_library-common-dae cpt_yahsp_library-dae)

        ADD_EXECUTABLE(test_stats test_stats.cpp)
        ADD_DEPENDENCIES(test_stats daex)
        TARGET_LINK_LIBRARIES(test_stats daex cpt_yahsp_library-common-dae cpt_yahsp_library-dae)

        ADD_EXECUTABLE(featureselector featureselector.cpp)
        ADD_DEPENDENCIES(featureselector daex)
        TARGET_LINK_LIBRARIES(featureselector daex cpt_yahsp_library-common-dae cpt_yahsp_library-dae)
    ENDIF()
ENDIF()

IF(DAE_WITH_MPI)
    ADD_EXECUTABLE(test_serial tests/test_serialization.cpp)
    ADD_DEPENDENCIES(test_serial daex)
    TARGET_LINK_LIBRARIES(test_serial daex cpt_yahsp_library-common
            cpt_yahsp_library-dae)
ENDIF()

# process to sources
ADD_SUBDIRECTORY(src)

