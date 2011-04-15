CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ENABLE_LANGUAGE(CXX)

# include the install configuration file where are defined the main variables
INCLUDE(${CMAKE_SOURCE_DIR}/install.cmake OPTIONAL)

# define project name
PROJECT(yahsp_library)

# include useful features for cmake
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

FIND_PACKAGE(OpenMP)

# include the sources
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

# flags to use with compiler
ADD_DEFINITIONS(-O3 -Wall -g)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# process to sources
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(application)
