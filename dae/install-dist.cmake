
# Variables to set

#  directory where ParadisEO has been installed
SET(EO_DIR "<<PATH_TO_EO>>" CACHE PATH "EO directory" FORCE)

# # directory where CPT has been installed
# SET(CPT_DIR "<<PATH_TO_CPT>>" CACHE PATH "CPT directory" FORCE)

# directory where YAHSP has been installed
SET(YAHSP_DIR "<<PATH_TO_YAHSP>>" CACHE PATH "YAHSP directory" FORCE)

# directory where GDSL has been installed
SET(GDSL_INCLUDE_DIR "<<PATH_TO_GDSL_INCLUDE_DIR>>")
SET(GDSL_LIBRARY "<<PATH_TO_GDSL_LIBRARY>>")

# automagically set parameters, do not edit

SET(EO_SRC_DIR "${EO_DIR}/src" CACHE PATH "EO source directory" FORCE)
SET(EO_BIN_DIR "${EO_DIR}/release" CACHE PATH "EO binary directory" FORCE)

# SET(CPT_SRC_DIR "${CPT_DIR}" CACHE PATH "CPT source directory" FORCE)
# SET(CPT_LIB_DIR "${CPT_DIR}/lib" CACHE PATH "CPT library directory" FORCE)

SET(YAHSP_SRC_DIR "${YAHSP_DIR}" CACHE PATH "YAHSP source directory" FORCE)
SET(YAHSP_BIN_DIR "${YAHSP_DIR}/release" CACHE PATH "YAHSP library directory" FORCE)
