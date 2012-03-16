# include the sources

#INCLUDE_DIRECTORIES(${EO_SRC_DIR}/src)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# target the dae lib

SET(DAEX_LIB_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
SET(LIBRARY_OUTPUT_PATH ${DAEX_LIB_OUTPUT_PATH})

SET (DAEX_SOURCES
  utils/posix_timer.cpp
  utils/pddl_load.cpp
  utils/evalBestPlanDump.cpp
  core/decomposition.cpp
  core/goal.cpp
  )

ADD_LIBRARY(daex STATIC ${DAEX_SOURCES})
ADD_LIBRARY(daex-yahsp-mt STATIC ${DAEX_SOURCES})

SET_TARGET_PROPERTIES(
  daex-yahsp-mt
  PROPERTIES 
  COMPILE_FLAGS "-DYAHSP_MT"
  )

