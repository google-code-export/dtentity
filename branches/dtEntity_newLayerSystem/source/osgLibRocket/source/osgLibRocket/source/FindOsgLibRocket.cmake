# - Find osgLibRocket


MACRO(FIND_OSGLIBROCKET_LIBRARY_WITH_DEBUG find_var libname paths)

  IF( "${paths}" STREQUAL "" )
    FIND_LIBRARY(${find_var}_RELEASE NAMES ${libname} PATHS ${CMAKE_LIBRARY_PATH})
    FIND_LIBRARY(${find_var}_DEBUG NAMES ${libname}d ${libname}_d PATHS ${CMAKE_LIBRARY_PATH})
  ELSE( "${paths}" STREQUAL "" )
    FIND_LIBRARY(${find_var}_RELEASE NAMES ${libname} PATHS ${paths} ${CMAKE_LIBRARY_PATH})
    FIND_LIBRARY(${find_var}_DEBUG NAMES ${libname}d ${libname}_d PATHS ${paths} ${CMAKE_LIBRARY_PATH})
  ENDIF( "${paths}" STREQUAL "" )

  # release and debug found
  IF(${find_var}_RELEASE AND ${find_var}_DEBUG)
    SET(${find_var} optimized ${${find_var}_RELEASE} debug ${${find_var}_DEBUG} CACHE STRING "")
  ELSE(${find_var}_RELEASE AND ${find_var}_DEBUG)
     # release found
     IF(${find_var}_RELEASE)
      SET(${find_var} ${${find_var}_RELEASE})
    ELSE(${find_var}_RELEASE)
      # debug found
      IF(${find_var}_DEBUG)
        SET(${find_var} ${${find_var}_DEBUG})
      ENDIF(${find_var}_DEBUG)
    ENDIF(${find_var}_RELEASE)
  ENDIF(${find_var}_RELEASE AND ${find_var}_DEBUG)

  MARK_AS_ADVANCED(${find_var} ${find_var}_RELEASE ${find_var}_DEBUG)

ENDMACRO(FIND_DTENTITY_LIBRARY_WITH_DEBUG)


IF(OSGLIBROCKET_INCLUDE_DIR AND OSGLIBROCKET_LIBRARIES)

  # in cache already
  SET(OSGLIBROCKET_FOUND TRUE)

ELSE(OSGLIBROCKET_INCLUDE_DIR AND OSGLIBROCKET_LIBRARIES)

  INCLUDE(FindPackageHandleStandardArgs)
  set(OSGLIBROCKET_FIND_PATHS ${CMAKE_INCLUDE_PATH} /usr/include /usr/local/include)
  FIND_PATH(DTENTITY_INCLUDE_DIR osgLibRocket/FileInterface PATHS ${OSGLIBROCKET_FIND_PATHS})
  FIND_OSGLIBROCKET_LIBRARY_WITH_DEBUG(OSGLIBROCKET_LIBRARY osgLibRocket "")

ENDIF(OSGLIBROCKET_INCLUDE_DIR AND OSGLIBROCKET_LIBRARIES)


