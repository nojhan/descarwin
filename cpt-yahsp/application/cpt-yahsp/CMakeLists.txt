PROJECT(cpt_yahsp CXX)

SET(SOURCES
  main.cpp
  )

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} cpt_yahsp_library-common cpt_yahsp_library)

SET_TARGET_PROPERTIES(
  ${PROJECT_NAME}
  PROPERTIES
  COMPILE_FLAGS ""
  LINK_FLAGS "-static"
  )

if(OPENMP_FOUND)

  ADD_EXECUTABLE(${PROJECT_NAME}-mt ${SOURCES})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME}-mt cpt_yahsp_library-common-omp cpt_yahsp_library-mt)

  SET_TARGET_PROPERTIES(
    ${PROJECT_NAME}-mt
    PROPERTIES
    COMPILE_FLAGS "${OpenMP_C_FLAGS}"
    LINK_FLAGS "${OpenMP_C_FLAGS} -static"
    )

endif()

if(MPI)

  ADD_EXECUTABLE(${PROJECT_NAME}-mpi ${SOURCES})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME}-mpi cpt_yahsp_library-common-mpi cpt_yahsp_library-mpi)

  SET_TARGET_PROPERTIES(
    ${PROJECT_NAME}-mpi
    PROPERTIES
    COMPILE_FLAGS "-DYAHSP_MPI"
    LINK_FLAGS "-static"
    )

endif()

# ADD_EXECUTABLE(${PROJECT_NAME}-wct ${SOURCES})
# TARGET_LINK_LIBRARIES(${PROJECT_NAME}-wct cpt_yahsp_library-wct)


# SET_TARGET_PROPERTIES(
#   ${PROJECT_NAME}-wct
#   PROPERTIES
#   COMPILE_FLAGS "${OpenMP_CXX_FLAGS} -DWALLCLOCK_TIME"
#   LINK_FLAGS "${OpenMP_C_FLAGS}"
#   )
