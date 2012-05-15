
# Variables to set

#  directory where EO has been installed
SET(EO_DIR "<<PATH_TO_EO>>" CACHE PATH "EO directory" FORCE)

# automagically set parameters, do not edit

SET(EO_SRC_DIR "${EO_DIR}/src" CACHE PATH "EO source directory" FORCE)

# If you plan to use MPI, precise here where are the static libraries from
# openmpi and boost::mpi.

SET(BOOST_LIB_DIR "<< PATH_TO_BOOST_STATIC_LIB >>" CACHE PATH "Boost library directory" FORCE)
SET(BOOST_INC_DIR "<< PATH_TO_BOOST_INCLUDE_LIB >>" CACHE PATH "Boost include directory" FORCE)

SET(MPI_DIR "<< PATH_TO_OPEN_MPI_STATIC >>" CACHE PATH "OpenMPI directory" FORCE)
