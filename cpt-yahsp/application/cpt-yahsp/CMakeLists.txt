PROJECT(cpt_yahsp CXX)

SET(SOURCES
  main.cpp
  )

LINK_LIBRARIES(${GDSL_LIBRARY})

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES})
ADD_DEPENDENCIES(${PROJECT_NAME} cpt_yahsp_library-common cpt_yahsp_library-dae)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} cpt_yahsp_library-dae)

ADD_EXECUTABLE(${PROJECT_NAME}-mt ${SOURCES})
ADD_DEPENDENCIES(${PROJECT_NAME}-mt cpt_yahsp_library-common cpt_yahsp_library-mt)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}-mt cpt_yahsp_library-mt)

ADD_EXECUTABLE(${PROJECT_NAME}-sm ${SOURCES})
ADD_DEPENDENCIES(${PROJECT_NAME}-sm cpt_yahsp_library-common cpt_yahsp_library-sm)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}-sm cpt_yahsp_library-sm)

ADD_EXECUTABLE(${PROJECT_NAME}-mt-sm ${SOURCES})
ADD_DEPENDENCIES(${PROJECT_NAME}-mt-sm cpt_yahsp_library-common cpt_yahsp_library-mt-sm)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}-mt-sm cpt_yahsp_library-mt-sm)

# ADD_EXECUTABLE(${PROJECT_NAME}-wct ${SOURCES})
# ADD_DEPENDENCIES(${PROJECT_NAME}-wct cpt_yahsp_library-wct)
# TARGET_LINK_LIBRARIES(${PROJECT_NAME}-wct cpt_yahsp_library-wct)

SET_TARGET_PROPERTIES(
  ${PROJECT_NAME}
  ${PROJECT_NAME}-mt
  ${PROJECT_NAME}-sm
  ${PROJECT_NAME}-mt-sm
  PROPERTIES
  COMPILE_FLAGS "${OpenMP_C_FLAGS}"
  LINK_FLAGS "${OpenMP_C_FLAGS}"
  )

# SET_TARGET_PROPERTIES(
#   ${PROJECT_NAME}-wct
#   PROPERTIES
#   COMPILE_FLAGS "${OpenMP_CXX_FLAGS} -DWALLCLOCK_TIME"
#   LINK_FLAGS "${OpenMP_C_FLAGS}"
#   )
