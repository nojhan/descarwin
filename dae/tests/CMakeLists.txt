
IF( CPT_WITH_OMP )
# Debug + OMP
    set( COMMON_LIBS cpt_yahsp_library-common-dae-omp cpt_yahsp_library-dae)
ELSE()
# Debug without OMP
    set( COMMON_LIBS cpt_yahsp_library-common-dae cpt_yahsp_library-dae)
ENDIF()

ADD_EXECUTABLE(test_chronopartition test_chronopartition.cpp)
ADD_DEPENDENCIES(test_chronopartition daex)
TARGET_LINK_LIBRARIES(test_chronopartition daex ${COMMON_LIBS} )

ADD_EXECUTABLE(test_b_max test_b_max.cpp)
ADD_DEPENDENCIES(test_b_max daex)
TARGET_LINK_LIBRARIES(test_b_max daex ${COMMON_LIBS})

ADD_EXECUTABLE(test_stats test_stats.cpp)
ADD_DEPENDENCIES(test_stats daex)
TARGET_LINK_LIBRARIES(test_stats daex ${COMMON_LIBS})

ADD_EXECUTABLE(test_mo_variation test_mo_variation.cpp)
ADD_DEPENDENCIES(test_mo_variation daex)
TARGET_LINK_LIBRARIES(test_mo_variation daex ${COMMON_LIBS})


IF(DAE_WITH_MPI)
    ADD_EXECUTABLE(test_serial test_serialization.cpp)
    ADD_DEPENDENCIES(test_serial daex)
    TARGET_LINK_LIBRARIES(test_serial daex cpt_yahsp_library-common cpt_yahsp_library-dae)
ENDIF()
