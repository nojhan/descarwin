cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


# include the install configuration file where are defined the main variables
INCLUDE(${CMAKE_SOURCE_DIR}/install.cmake)


# define project name
PROJECT(DAEX CXX)

# include the sources

INCLUDE_DIRECTORIES(${EO_SRC_DIR})
INCLUDE_DIRECTORIES(${CPT_SRC_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)


# where CMake can find the libraries

LINK_DIRECTORIES(${EO_BIN_DIR}/lib)
LINK_DIRECTORIES(${CPT_LIB_DIR})

SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DPAPERVERSION -fopenmp" )
SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG -fopenmp" )

# make a static exe
# FIXME this is a ugly ha as it assume that the linker is gcc-compatible
# there must be a more elegant way to do that with CMake, maybe with:
# LINK_SEARCH_END_STATIC ?
SET(CMAKE_EXE_LINKER_FLAGS "-fopenmp")

# target

ADD_EXECUTABLE(dae dae.cpp) 
ADD_DEPENDENCIES(dae daex)

ADD_EXECUTABLE(dae-mt dae.cpp) 
ADD_DEPENDENCIES(dae-mt daex)

ADD_EXECUTABLE(test_chronopartition test_chronopartition.cpp) 
ADD_DEPENDENCIES(test_chronopartition daex)

ADD_EXECUTABLE(test_b_max test_b_max.cpp) 
ADD_DEPENDENCIES(test_b_max daex)

ADD_EXECUTABLE(test_stats test_stats.cpp) 
ADD_DEPENDENCIES(test_stats daex)

# link librairies for the executable

TARGET_LINK_LIBRARIES(dae eoutils daex cpt gmp eo gcov) 
TARGET_LINK_LIBRARIES(dae-mt eoutils daex cpt-mt gmp eo gcov) 
TARGET_LINK_LIBRARIES(test_chronopartition eoutils daex cpt gmp eo gcov) 
TARGET_LINK_LIBRARIES(test_b_max eoutils daex cpt gmp eo gcov) 
TARGET_LINK_LIBRARIES(test_stats eoutils daex cpt gmp eo gcov) 


# process to sources

SUBDIRS(src)

