# - Find dtEntity


MACRO(FIND_DTENTITY_LIBRARY_WITH_DEBUG find_var libname paths)

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


IF(DTENTITY_INCLUDE_DIR AND DTENTITY_LIBRARIES)

  # in cache already
  SET(DTENTITY_FOUND TRUE)

ELSE(DTENTITY_INCLUDE_DIR AND DTENTITY_LIBRARIES)

  INCLUDE(FindPackageHandleStandardArgs)
  set(DTENTITY_FIND_PATHS ${CMAKE_INCLUDE_PATH} /usr/include /usr/local/include)
  FIND_PATH(DTENTITY_INCLUDE_DIR dtEntity/entitymanager.h PATHS ${DTENTITY_FIND_PATHS})
  FIND_PATH(DTENTITY_BUILD_INCLUDE_DIR dtEntity/dtentity_config.h PATHS ${DTENTITY_FIND_PATHS})
  FIND_DTENTITY_LIBRARY_WITH_DEBUG(DTENTITY_LIBRARY dtEntity "")
  FIND_DTENTITY_LIBRARY_WITH_DEBUG(DTENTITY_QT_WIDGETS_LIBRARY dtEntityQtWidgets "")
  FIND_DTENTITY_LIBRARY_WITH_DEBUG(DTENTITY_WRAPPERS_LIBRARY dtEntityWrappers "")
  FIND_DTENTITY_LIBRARY_WITH_DEBUG(DTENTITY_NET_LIBRARY dtEntityNet "")

ENDIF(DTENTITY_INCLUDE_DIR AND DTENTITY_LIBRARIES)


