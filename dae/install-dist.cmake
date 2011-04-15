
# Variables to set

#  directory where EO has been installed
SET(EO_DIR "<<PATH_TO_EO>>" CACHE PATH "EO directory" FORCE)

# directory where CPT-YAHSP has been installed
SET(CPT_YAHSP_DIR "<<PATH_TO_CPT_YAHSP>>" CACHE PATH "CPT-YAHSP directory" FORCE)

# directory where GDSL has been installed
# these variables must be only defined if you are using a customized installation of GDSL
#SET(GDSL_INCLUDE_DIR "<<PATH_TO_GDSL_INCLUDE_DIR>>")
#SET(GDSL_LIBRARY "<<PATH_TO_GDSL_LIBRARY>>")

# automagically set parameters, do not edit

SET(EO_SRC_DIR "${EO_DIR}/src" CACHE PATH "EO source directory" FORCE)
SET(EO_BIN_DIR "${EO_DIR}/release" CACHE PATH "EO binary directory" FORCE)

SET(CPT_YAHSP_SRC_DIR "${CPT_YAHSP_DIR}" CACHE PATH "CPT-YAHSP source directory" FORCE)
SET(CPT_YAHSP_BIN_DIR "${CPT_YAHSP_DIR}/release" CACHE PATH "CPT-YAHSP library directory" FORCE)
