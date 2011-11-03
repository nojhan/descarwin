
# Variables to set
SET(CMAKE_BUILD_TYPE Debug CACHE TYPE INTERNAL FORCE)
#  directory where EO has been installed
#SET(EO_DIR "/home/mustapha/framework/dae/eo" CACHE PATH "EO directory" FORCE)

# directory where CPT-YAHSP has been installed
SET(CPT_YAHSP_DIR "/home/mustapha/framework/dae/descarwin/trunk/cpt-yahsp" CACHE PATH "CPT-YAHSP directory" FORCE)

# directory where GDSL has been installed
# these variables must be only defined if you are using a customized installation of GDSL
 
SET(GDSL_INCLUDE_DIR "/home/mustapha/framework/dae/descarwin/trunk/cpt-yahsp/gdsl/src")
SET(GDSL_LIBRARY "/home/mustapha/framework/dae/descarwin/trunk/cpt-yahsp/gdsl/src")
# automagically set parameters, do not edit

#SET(EO_SRC_DIR "${EO_DIR}/src" CACHE PATH "EO source directory" FORCE)
#SET(EO_BIN_DIR "${EO_DIR}/release" CACHE PATH "EO binary directory" FORCE)

SET(CPT_YAHSP_SRC_DIR "${CPT_YAHSP_DIR}" CACHE PATH "CPT-YAHSP source directory" FORCE)
SET(CPT_YAHSP_BIN_DIR "${CPT_YAHSP_DIR}/release" CACHE PATH "CPT-YAHSP library directory" FORCE)




SET(DAE_DIR "/home/mustapha/framework/dae/descarwin/trunk/dae" CACHE PATH "DAE directory" FORCE)

SET(DAE_SRC_DIR "${DAE_DIR}/src" CACHE PATH "DAE source directory" FORCE)
SET(DAE_BIN_DIR "${DAE_DIR}/lib" CACHE PATH "DAE library directory" FORCE)



SET(PARADISEO_DIR "/home/mustapha/framework/paradiseo-1.2.1" CACHE PATH "ParadisEO directory" FORCE)


#SET(PARADISEO_EO_SRC_DIR "${EO_DIR}" CACHE PATH "ParadisEO-EO source directory" FORCE)
#SET(PARADISEO_EO_BIN_DIR "${EO_DIR}/release" CACHE PATH "ParadisEO-EO binary directory" FORCE)

SET(PARADISEO_EO_SRC_DIR "${PARADISEO_DIR}/paradiseo-eo" CACHE PATH "ParadisEO-EO source directory" FORCE)
SET(PARADISEO_EO_BIN_DIR "${PARADISEO_DIR}/paradiseo-eo/build" CACHE PATH "ParadisEO-EO binary directory" FORCE)

SET(PARADISEO_MO_SRC_DIR "${PARADISEO_DIR}/paradiseo-mo" CACHE PATH "ParadisEO-MO source directory" FORCE)
SET(PARADISEO_MO_BIN_DIR "${PARADISEO_DIR}/paradiseo-mo/build" CACHE PATH "ParadisEO-MO binary directory" FORCE)

SET(PARADISEO_MOEO_SRC_DIR "${PARADISEO_DIR}/paradiseo-moeo" CACHE PATH "ParadisEO-MOEO source directory" FORCE)
SET(PARADISEO_MOEO_BIN_DIR "${PARADISEO_DIR}/paradiseo-moeo/build" CACHE PATH "ParadisEO-MOEO binary directory" FORCE)


SET(PARADISEO_PEO_SRC_DIR "${PARADISEO_DIR}/paradiseo-peo" CACHE PATH "ParadisEO-PEO source directory" FORCE)
SET(PARADISEO_PEO_BIN_DIR "${PARADISEO_DIR}/paradiseo-peo/build" CACHE PATH "ParadisEO-PEO binary directory" FORCE)

