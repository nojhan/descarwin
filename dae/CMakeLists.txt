cmake_minimum_required(VERSION 2.8)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# include the install configuration file where are defined the main variables
INCLUDE(${CMAKE_SOURCE_DIR}/install.cmake)

# define project name
PROJECT(DAEX)

# include useful features for cmake
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

FIND_PACKAGE(OpenMP REQUIRED)
FIND_PACKAGE(GDSL REQUIRED)

IF(OPENMP_FOUND)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
ENDIF()

# include the sources
INCLUDE_DIRECTORIES(
  ${EO_SRC_DIR}
  ${CPT_YAHSP_SRC_DIR}
  ${GDSL_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/src
  )

# where CMake can find the libraries
LINK_DIRECTORIES(
  ${EO_BIN_DIR}/lib
  ${CPT_YAHSP_BIN_DIR}/lib
  )

# flags to use with compiler
ADD_DEFINITIONS(-W -Wall)

IF (CMAKE_BUILD_TYPE MATCHES Debug)
  #ADD_DEFINITIONS(-DPAPERVERSION)
  ADD_DEFINITIONS(-pg -g)
ELSE()
  ADD_DEFINITIONS(-O3 -DNDEBUG)
ENDIF()

# target + link libraries
LINK_LIBRARIES(eoutils daex gmp eo gcov ${GDSL_LIBRARY} yahsp_library-common)

ADD_EXECUTABLE(dae dae.cpp)
ADD_DEPENDENCIES(dae daex)
TARGET_LINK_LIBRARIES(dae yahsp_library-dae)

ADD_EXECUTABLE(dae-sm dae.cpp)
ADD_DEPENDENCIES(dae-sm daex)
TARGET_LINK_LIBRARIES(dae-sm yahsp_library-sm)

ADD_EXECUTABLE(dae-mt dae.cpp)
ADD_DEPENDENCIES(dae-mt daex)
TARGET_LINK_LIBRARIES(dae-mt yahsp_library-mt)

ADD_EXECUTABLE(dae-mt-sm dae.cpp)
ADD_DEPENDENCIES(dae-mt-sm daex)
TARGET_LINK_LIBRARIES(dae-mt-sm yahsp_library-mt-sm)

ADD_EXECUTABLE(test_chronopartition test_chronopartition.cpp)
ADD_DEPENDENCIES(test_chronopartition daex)
TARGET_LINK_LIBRARIES(test_chronopartition yahsp_library-dae)

ADD_EXECUTABLE(test_b_max test_b_max.cpp)
ADD_DEPENDENCIES(test_b_max daex)
TARGET_LINK_LIBRARIES(test_b_max yahsp_library-dae)

ADD_EXECUTABLE(test_stats test_stats.cpp)
ADD_DEPENDENCIES(test_stats daex)
TARGET_LINK_LIBRARIES(test_stats yahsp_library-dae)

ADD_EXECUTABLE(featureselector featureselector.cpp)
ADD_DEPENDENCIES(featureselector daex)
TARGET_LINK_LIBRARIES(featureselector yahsp_library-dae)

# process to sources
ADD_SUBDIRECTORY(src)
