# - Find Cal3D
#   Find Cal3D includes and library
#
#  CAL3D_INCLUDE_DIR - where to find Cal3D includes.
#  CAL3D_LIBRARIES   - List of libraries when using Cal3D.
#  CAL3D_FOUND       - True if Cal3D found.

IF(CAL3D_INCLUDE_DIR AND CAL3D_LIBRARIES)

  # in cache already
  SET(CAL3D_FOUND TRUE)

ELSE(CAL3D_INCLUDE_DIR AND CAL3D_LIBRARIES)

  INCLUDE(FindPackageHandleStandardArgs)
  INCLUDE(FindLibraryWithDebug)
  
  FIND_PATH(CAL3D_INCLUDE_DIR cal3d/cal3d.h PATHS ${CMAKE_INCLUDE_PATH})
  FIND_LIBRARY_WITH_DEBUG(CAL3D_LIBRARIES cal3d "")

ENDIF(CAL3D_INCLUDE_DIR AND CAL3D_LIBRARIES)
