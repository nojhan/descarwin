CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# include the install configuration file where are defined the main variables
INCLUDE(${CMAKE_SOURCE_DIR}/install.cmake OPTIONAL)

PROJECT(DESCARWIN)

# include useful features for cmake
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

FIND_PACKAGE(GDSL REQUIRED)

# include the sources
INCLUDE_DIRECTORIES(
  ${GDSL_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/cpt-yahsp
  ${CMAKE_SOURCE_DIR}/cpt-yahsp/src
  ${CMAKE_SOURCE_DIR}/dae/src
  )

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
# SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

ADD_SUBDIRECTORY(cpt-yahsp)
ADD_SUBDIRECTORY(dae)
