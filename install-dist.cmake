
# Variables to set

#  directory where EO has been installed
SET(EO_DIR "<<PATH_TO_EO>>" CACHE PATH "EO directory" FORCE)

# directory where GDSL has been installed
# these variables must be only defined if you are using a customized installation of GDSL
#SET(GDSL_INCLUDE_DIR "<<PATH_TO_GDSL_INCLUDE_DIR>>")
#SET(GDSL_LIBRARY "<<PATH_TO_GDSL_LIBRARY>>")

# automagically set parameters, do not edit

SET(EO_SRC_DIR "${EO_DIR}/src" CACHE PATH "EO source directory" FORCE)
SET(EO_BIN_DIR "${EO_DIR}/release" CACHE PATH "EO binary directory" FORCE)
