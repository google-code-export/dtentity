# - Find OPENAL
#   Find OPENAL includes and library
#

IF(OPENAL_INCLUDE_DIR AND OPENAL_LIBRARIES)

  # in cache already
  SET(OPENAL_FOUND TRUE)

ELSE(OPENAL_INCLUDE_DIR AND OPENAL_LIBRARIES)

  INCLUDE(FindPackageHandleStandardArgs)
  INCLUDE(FindLibraryWithDebug)

  set(OPENAL_INC_PATHS
    /usr/include
    /usr/local
    /usr
	${CMAKE_INCLUDE_PATH}
  )
  FIND_PATH(OPENAL_INCLUDE_DIR AL/al.h PATHS ${OPENAL_INC_PATHS})

  #look for the OpenAL32 library
#  FIND_LIBRARY(OPENAL_LIBRARIES NAMES OpenAL32 PATHS $ENV{VIEWER_EXT_DEP}/lib)
  FIND_LIBRARY_WITH_DEBUG(OPENAL_LIBRARIES openal "")
  FIND_LIBRARY_WITH_DEBUG(OPENAL_LIBRARIES OpenAL32-Soft "")
  # look for the alut libraries
  FIND_LIBRARY_WITH_DEBUG(OPENAL_UTIL_LIBRARIES alut "")
  FIND_LIBRARY_WITH_DEBUG(OPENAL_UTIL_LIBRARIES alut-soft "")
  
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(OPENAL DEFAULT_MSG OPENAL_LIBRARIES OPENAL_INCLUDE_DIR)

ENDIF(OPENAL_INCLUDE_DIR AND OPENAL_LIBRARIES)

