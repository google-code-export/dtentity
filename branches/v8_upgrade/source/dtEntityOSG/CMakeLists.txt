SET(LIB_NAME dtEntityOSG)

FIND_PACKAGE(OsgEphemeris)

INCLUDE_DIRECTORIES( 
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
  ${OSG_INCLUDE_DIR}
)

SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include/${LIB_NAME})


# generate configuration header file
SET(DTENTITYOSG_CONFIG_HEADER "${PROJECT_BINARY_DIR}/include/dtEntityOSG/dtentityosg_config.h")
configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/dtentityosg_config.h.in"
  ${DTENTITYOSG_CONFIG_HEADER}
)

SET(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/cameracomponent.h
  ${HEADER_PATH}/componentfactories.h
  ${HEADER_PATH}/export.h
  ${HEADER_PATH}/groundclampingcomponent.h
  ${HEADER_PATH}/groupcomponent.h
  ${HEADER_PATH}/initosgviewer.h
  ${HEADER_PATH}/layerattachpointcomponent.h
  ${HEADER_PATH}/layercomponent.h
  ${HEADER_PATH}/lightcomponent.h
  ${HEADER_PATH}/matrixtransformcomponent.h
  ${HEADER_PATH}/manipulatorcomponent.h
  ${HEADER_PATH}/manipulators.h
  ${HEADER_PATH}/nodecomponent.h
  ${HEADER_PATH}/nodemaskvisitor.h
  ${HEADER_PATH}/osganimationcomponent.h
  ${HEADER_PATH}/osgdebugdrawinterface.h
  ${HEADER_PATH}/osginputinterface.h
  ${HEADER_PATH}/osgsysteminterface.h
  ${HEADER_PATH}/osgwindowinterface.h
  ${HEADER_PATH}/particlecomponent.h
  ${HEADER_PATH}/pathcomponent.h
  ${HEADER_PATH}/pickshapecomponent.h
  ${HEADER_PATH}/positionattitudetransformcomponent.h
  ${HEADER_PATH}/resourcemanager.h
  ${HEADER_PATH}/shadercomponent.h
  ${HEADER_PATH}/shadowcomponent.h
  ${HEADER_PATH}/skyboxcomponent.h
  ${HEADER_PATH}/staticmeshcomponent.h
  ${HEADER_PATH}/textlabelcomponent.h
  ${HEADER_PATH}/texturelabelcomponent.h
  ${HEADER_PATH}/transformcomponent.h
  ${DTENTITYOSG_CONFIG_HEADER}
)


SET(LIB_SOURCES
  componentfactories.cpp
  initosgviewer.cpp
)

SET(LIB_SOURCES_REPLACE  
  cameracomponent.cpp
  groundclampingcomponent.cpp
  groupcomponent.cpp
  layerattachpointcomponent.cpp
  layercomponent.cpp
  lightcomponent.cpp
  manipulatorcomponent.cpp
  manipulators.cpp
  matrixtransformcomponent.cpp
  nodecomponent.cpp
  osganimationcomponent.cpp
  osgdebugdrawinterface.cpp
  osginputinterface.cpp
  osgsysteminterface.cpp
  osgwindowinterface.cpp
  pathcomponent.cpp
  particlecomponent.cpp
  pickshapecomponent.cpp
  positionattitudetransformcomponent.cpp
  resourcemanager.cpp
  shadercomponent.cpp
  shadowcomponent.cpp
  skyboxcomponent.cpp
  staticmeshcomponent.cpp
  textlabelcomponent.cpp
  texturelabelcomponent.cpp  
  transformcomponent.cpp
)


SET(WRAPLIBRARIES dtEntity
                  ${OPENSCENEGRAPH_LIBRARIES}
                  ${OPENTHREADS_LIBRARIES}
)


IF(OSGEPHEMERIS_FOUND)
  INCLUDE_DIRECTORIES(${OSGEPHEMERIS_INCLUDE_DIR})
  LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/osgephemeriscomponent.h)
  LIST(APPEND LIB_SOURCES osgephemeriscomponent.cpp)
  LIST(APPEND WRAPLIBRARIES ${OSGEPHEMERIS_LIBRARIES})
ENDIF(OSGEPHEMERIS_FOUND)

REPLACE_SIDS_IN_SOURCE(LIB_SOURCES_OUT ${LIB_SOURCES_REPLACE})

ADD_LIBRARY(${LIB_NAME} ${DTENTITY_LIBS_DYNAMIC_OR_STATIC}
  ${LIB_PUBLIC_HEADERS}
  ${LIB_SOURCES}
  ${LIB_SOURCES_OUT}
  ${LIB_SOURCES_REPLACE}
)

TARGET_LINK_LIBRARIES( ${LIB_NAME} ${WRAPLIBRARIES})

SET_TARGET_PROPERTIES(${LIB_NAME}	PROPERTIES DEFINE_SYMBOL DTENTITY_OSG_LIBRARY)
SETUP_LINK_LIBRARIES(${LIB_NAME})
INCLUDE(ModuleInstall OPTIONAL)

IF (MSVC)
  ADD_DEFINITIONS(-DNODEFAULTLIB:LIBCMT)
  ADD_DEFINITIONS(-DNODEFAULTLIB:LIBCMTD)
ENDIF (MSVC)

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")


