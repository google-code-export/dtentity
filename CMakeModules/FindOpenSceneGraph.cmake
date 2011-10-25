# Locate and setup OpenSceneGraph

# This module defines
# OSG_FOUND        - TRUE when OpenSceneGraph is found
# OSG_INCLUDE_DIRS - path(s) to OpenSceneGraph headers
# OSG_LIBRARIES    - all OpenSceneGraph libraries you wanted to use
#
# define USE_OSG_foo with foo ==
#    DB
#    FX
#    GA
#    MANIPULATOR
#    PARTICLE
#    SHADOW
#    SIM
#    TERRAIN
#    TEXT
#    UTIL
#    VIEWER
#
# to search for the specific OpenSceneGraph libraries. They'll be added to OSG_LIBRARIES
#
# $OSG_DIR is an environment variable that would
# correspond to the ./configure --prefix=$OSG_DIR
#
# Created by Christian Ehrlicher

INCLUDE(FindPackageHandleStandardArgs)
INCLUDE(FindLibraryWithDebug)


SET(OSG_LIBRARIES CACHE STRING "OpenSceneGraph libraries to link against")
SET(OSG_ROOT_DIR $ENV{OSG_DIR} CACHE PATH "Root directory of OSG source tree")
SET(OSG_BUILD_DIR ${OSG_ROOT_DIR} CACHE PATH "Root directory for OSG build to use")

FIND_PATH(OSG_INCLUDE_DIRS osg/Program PATHS
   ${OSG_ROOT_DIR}/include
   $ENV{OSG_DIR}
   $ENV{OSG_DIR}/include
	$ENV{OSG_INC}
	$ENV{VIEWER_EXT_DEP}/include
	$ENV{DELTA3D_EXT}
	$ENV{DELTA3D_EXT}/lib
)

# Also add the build-specific includes
SET(OSG_INCLUDE_DIRS ${OSG_INCLUDE_DIRS} ${OSG_BUILD_DIR}/include)
message (STATUS "Added build-specific include dir for OpenSceneGraph...")

SET(OSG_AVAILABLE_LIBS
    DB
    FX
    GA
    Animation
    Manipulator
    Particle
    Shadow
    Sim
    Terrain
    Text
    Util
    Viewer
)

SET(libpath $ENV{DELTA3D_EXT}/lib ${OSG_BUILD_DIR}/lib)
IF(NOT "${libpath}" STREQUAL "")
  FILE(TO_CMAKE_PATH "${libpath}" libpath)
ENDIF(NOT "${libpath}" STREQUAL "")

SET(OSG_FIND_PATHS ${OSG_BUILD_DIR}/lib
                   ${OSG_INCLUDE_DIRS}/../lib
                   ${OSG_INCLUDE_DIRS}/../lib64
                   ${libpath}
)

FIND_LIBRARY_WITH_DEBUG(OSG_LIBRARY osg "${OSG_FIND_PATHS}")

FIND_PACKAGE_HANDLE_STANDARD_ARGS(OSG DEFAULT_MSG OSG_LIBRARY OSG_INCLUDE_DIRS)
IF(OSG_LIBRARY)
  LIST(APPEND OSG_LIBRARIES ${OSG_LIBRARY})
ENDIF(OSG_LIBRARY)

FOREACH(osgLib ${OSG_AVAILABLE_LIBS})
  STRING(TOUPPER ${osgLib} osgLibUpper)

  IF(USE_OSG_${osgLibUpper})
    SET(osgLibName osg${osgLib})

    FIND_LIBRARY_WITH_DEBUG(OSG_${osgLibUpper}_LIBRARY ${osgLibName} "${OSG_FIND_PATHS}")

    IF(OSG_FIND_REQUIRED)
      SET(${osgLibName}_FIND_REQUIRED 1)
    ENDIF(OSG_FIND_REQUIRED)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(${osgLibName} DEFAULT_MSG OSG_${osgLibUpper}_LIBRARY)

    IF(OSG_${osgLibUpper}_LIBRARY)
      LIST(APPEND OSG_LIBRARIES ${OSG_${osgLibUpper}_LIBRARY})
    ENDIF(OSG_${osgLibUpper}_LIBRARY)

  ENDIF(USE_OSG_${osgLibUpper})
ENDFOREACH(osgLib)
