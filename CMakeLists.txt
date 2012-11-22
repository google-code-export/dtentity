CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)
 
PROJECT(dtEntity)


IF (WIN32)
  SET(CMAKE_DEBUG_POSTFIX  "d")
ENDIF (WIN32)

# define _DEBUG as set on debug builds on linux
IF(UNIX AND NOT WIN32)
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
ENDIF()

# append "64" to lib name if built for 64 bits
IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
    SET(SYSTEM_ARCHITECTURE "64bits")
    IF(UNIX AND NOT WIN32 AND NOT APPLE AND NOT DEFINED LIB_POSTFIX)
        SET(LIB_POSTFIX "64")
    ENDIF(UNIX AND NOT WIN32 AND NOT APPLE AND NOT DEFINED LIB_POSTFIX)
ELSE(CMAKE_SIZEOF_VOID_P MATCHES "8")
    IF(NOT DEFINED SYSTEM_ARCHITECTURE)
      SET(SYSTEM_ARCHITECTURE "32bits")
    ENDIF(NOT DEFINED SYSTEM_ARCHITECTURE)

    IF(NOT DEFINED LIB_POSTFIX)
       SET(LIB_POSTFIX "")
    ENDIF(NOT DEFINED LIB_POSTFIX)
ENDIF(CMAKE_SIZEOF_VOID_P MATCHES "8")
MARK_AS_ADVANCED(LIB_POSTFIX)

# where CMake should look for find modules
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_SOURCE_DIR}/source/osgLibRocket/cmake;${CMAKE_MODULE_PATH}")

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

INCLUDE(DTEntityMacroUtils)

#on windows, try to use provided dtEntity external dependency folder
IF(WIN32)
  FIND_PACKAGE(DTENTITY_EXT_DEP REQUIRED)
  if(MSVC)
    if(${MSVC_VERSION} STREQUAL 1600 OR ${MSVC_VERSION} STRGREATER 1600)
      message("Visual Studio 2010 (${MSVC_VERSION}) build fix at play (/FORCE:MULTIPLE)")
      set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /FORCE:MULTIPLE")
    endif()
  endif() 
ENDIF(WIN32)



# OSG is a required dependency
FIND_PACKAGE(OpenSceneGraph REQUIRED osgAnimation osgDB osgGA osgParticle osgShadow osgSim osgText osgUtil osgViewer osgManipulator)
FIND_PACKAGE(OpenThreads)

# Dynamic vs Static Linking
OPTION(DTENTITY_LIBRARY_STATIC "Set to OFF to build dtEntity for dynamic linking.  Use ON for static." OFF)
IF(DTENTITY_LIBRARY_STATIC)
    SET(DTENTITY_PLUGINS_DYNAMIC_OR_STATIC "STATIC")
    SET(DTENTITY_LIBS_DYNAMIC_OR_STATIC "STATIC")
    SET(BUILD_OSGLIBROCKET_STATIC "TRUE")
ELSE ()
    SET(DTENTITY_PLUGINS_DYNAMIC_OR_STATIC "MODULE")
    SET(DTENTITY_LIBS_DYNAMIC_OR_STATIC "SHARED")
ENDIF()

OPTION(CALL_ONPROPERTYCHANGED_METHOD "When OFF, PropertyContainer::OnPropertyChanged is no longer called" ON)

# all plugins link against these libs
SET(TARGET_COMMON_LIBRARIES
    ${OPENTHREADS_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
    dtEntity
)

#PROJECT_BINARY_DIR for generated config header file
INCLUDE_DIRECTORIES(include ${OSG_INCLUDE_DIR} $ENV{OSG_DIR}/include ${PROJECT_BINARY_DIR}/include)

#to allow usage of hash_map without warnings:
IF(CMAKE_COMPILER_IS_GNUCXX)
  ADD_DEFINITIONS(${CMAKE_CXX_FLAGS} "-Wno-deprecated")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#apple support
if(APPLE) 
    if(NOT DTENTITY_CONFIG_HAS_BEEN_RUN_BEFORE)
        if ("${CMAKE_OSX_SYSROOT}" MATCHES ".*/Developer/SDKs/MacOSX10.7.sdk") 
            MESSAGE(STATUS "MacOSX10.7.sdk")
            SET(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architectures for OSX" FORCE)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.7 -fvisibility-inlines-hidden" CACHE STRING "Flags used by the compiler during all build types." FORCE)
        endif()    
     endif()
endif()

#should unit tests be built?
OPTION(BUILD_TESTS "Build unit tests" OFF)

# do a preprocessing step to replace all calls to function dtEntity::SID with the result of that function?
OPTION(DTENTITY_REPLACE_SIDS_WITH_PREPROCESSOR "Use a preprocessor to replace calls to dtEntity::SID with result of that operation (EXPERIMENTAL)" OFF)
IF(DTENTITY_REPLACE_SIDS_WITH_PREPROCESSOR)
  add_executable(HashSids source/hash_sids/hash_sids.cpp source/dtEntity/hash.cpp)#
  INSTALL(FILES ${CMAKE_BINARY_DIR}/sids.txt DESTINATION bin)
ENDIF(DTENTITY_REPLACE_SIDS_WITH_PREPROCESSOR)

ADD_SUBDIRECTORY(ext)
ADD_SUBDIRECTORY(source)

#should examples be built?
OPTION(BUILD_EXAMPLES "Build examples" OFF)
IF(BUILD_EXAMPLES)
  ADD_SUBDIRECTORY(examples)
ENDIF(BUILD_EXAMPLES)

