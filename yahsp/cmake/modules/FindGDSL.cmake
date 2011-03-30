# File: FindGDSL.cmake
# CMAKE commands to actually use the GDSL library
# Version: 0.0.1
#
# The following variables are filled out:
# - GDSL_INCLUDE_DIR
# - GDSL_LIBRARY
# - GDSL_FOUND
#

FIND_PATH(
  GDSL_INCLUDE_DIR
  gdsl.h
  PATHS
  /usr/include
  /usr/local/include
  )

FIND_LIBRARY(
  GDSL_LIBRARY
  NAMES gdsl
  PATHS
  /usr/lib
  /usr/local/lib
  )

IF(GDSL_INCLUDE_DIR)
  IF(GDSL_LIBRARY)
    SET(GDSL_FOUND "YES")
    MARK_AS_ADVANCED(GDSL_INCLUDE_DIR)
    MARK_AS_ADVANCED(GDSL_LIBRARY)
  ENDIF(GDSL_LIBRARY)
ENDIF(GDSL_INCLUDE_DIR)
