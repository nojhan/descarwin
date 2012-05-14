CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# include the install configuration file where are defined the main variables
INCLUDE(${CMAKE_SOURCE_DIR}/install.cmake OPTIONAL)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	MESSAGE("-- Compilation in debug mode")
	SET(EO_BUILD_DIR "debug")
ELSE()
	MESSAGE("-- Compilation in release mode")
	SET(EO_BUILD_DIR "release")
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

SET(EO_BIN_DIR "${EO_DIR}/${EO_BUILD_DIR}" CACHE PATH "EO binary directory" FORCE)

PROJECT(DESCARWIN)

# include useful features for cmake
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

# include the sources
INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/cpt-yahsp
  ${CMAKE_SOURCE_DIR}/cpt-yahsp/src
  ${CMAKE_SOURCE_DIR}/dae/src
  )

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
# SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

ADD_SUBDIRECTORY(cpt-yahsp)
ADD_SUBDIRECTORY(dae)
