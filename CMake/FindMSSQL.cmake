# - Find msodbcsql
# Find the native MSSQL includes and library
#
#  MSSQL_INCLUDE_DIR - where to find msodbcsql.h, etc.
#  MSSQL_LIBRARIES   - List of libraries when using MSSQL.
#  MSSQL_FOUND       - True if MSSQL found.

IF (MSSQL_INCLUDE_DIR)
  # Already in cache, be silent
  SET(MSSQL_FOUND_QUIETLY TRUE)
ENDIF ()

FIND_PATH(MSSQL_INCLUDE_DIR 
  NAMES msodbcsql.h sql.h
  /opt/microsoft/msodbcsql17/include
  /opt/microsoft/msodbcsql/include
  /usr/local/include
  /usr/include
)

FILE(GLOB MSSQL_LIBRARY_FILES /opt/microsoft/msodbcsql17/lib64/libmsodbcsql* /opt/microsoft/msodbcsql/lib64/libmsodbcsql*)
FOREACH(file ${MSSQL_LIBRARY_FILES})
    GET_FILENAME_COMPONENT(library ${file} NAME)
    LIST(APPEND MSSQL_NAMES ${library})
ENDFOREACH()
LIST(APPEND MSSQL_NAMES)
FIND_LIBRARY(MSSQL_LIBRARY
  NAMES ${MSSQL_NAMES}
  PATHS /opt/microsoft/msodbcsql17/lib64 /opt/microsoft/msodbcsql/lib64
)

IF (MSSQL_INCLUDE_DIR AND MSSQL_LIBRARY)
  SET(MSSQL_FOUND TRUE)
  SET( MSSQL_LIBRARIES ${MSSQL_LIBRARY} )
ELSE ()
  SET(MSSQL_FOUND FALSE)
  SET( MSSQL_LIBRARIES )
ENDIF ()

IF (MSSQL_FOUND)
  IF (NOT MSSQL_FOUND_QUIETLY)
    MESSAGE(STATUS "Found MSSQL: ${MSSQL_LIBRARY}")
  ENDIF ()
ELSE ()
  IF (MSSQL_FIND_REQUIRED)
    MESSAGE(STATUS "Looked for MSSQL libraries named ${MSSQL_NAMES}.")
    MESSAGE(FATAL_ERROR "Could NOT find MSSQL library")
  ENDIF ()
ENDIF ()

MARK_AS_ADVANCED(
  MSSQL_LIBRARY
  MSSQL_INCLUDE_DIR
)