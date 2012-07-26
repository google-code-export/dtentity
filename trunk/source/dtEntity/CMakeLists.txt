SET(LIB_NAME dtEntity)

IF (WIN32)
ADD_DEFINITIONS(-DNOMINMAX)
ENDIF (WIN32)

FIND_PACKAGE(OpenThreads REQUIRED)
FIND_PACKAGE(ProtoBuf)
FIND_PACKAGE(ENet)

OPTION(DTENTITY_USE_STRINGS_AS_STRINGIDS "Use std::string instead of hashed strings (for debugging)" OFF)

# Flag to build SoundSystem stuff (requires OpenAL)
OPTION(BUILD_OPENAL "Build OpenAL Sound module" OFF)
IF(BUILD_OPENAL)
  FIND_PACKAGE(OpenAL REQUIRED)
  INCLUDE_DIRECTORIES(
    ${OPENAL_INC_PATHS}
  )
ENDIF(BUILD_OPENAL)



OPTION(USE_BOOST_POOL "Use boost pool to store components" OFF)
IF(USE_BOOST_POOL)
  FIND_PACKAGE(Boost)
ENDIF(USE_BOOST_POOL)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/RapidXML
  ${OSG_INCLUDE_DIR}
  ${OPENTHREADS_INCLUDE_DIR}
)

SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# generate configuration header file
SET(DTENTITY_CONFIG_HEADER "${PROJECT_BINARY_DIR}/include/dtEntity/dtentity_config.h")
configure_file (
  "${SOURCE_PATH}/dtentity_config.h.in"
  ${DTENTITY_CONFIG_HEADER}
)

SET(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/applicationcomponent.h
  ${HEADER_PATH}/cameracomponent.h
  ${HEADER_PATH}/commandmessages.h
  ${HEADER_PATH}/component.h
  ${HEADER_PATH}/componentfactories.h
  ${HEADER_PATH}/componentplugin.h
  ${HEADER_PATH}/componentpluginmanager.h
  ${HEADER_PATH}/core.h
  ${HEADER_PATH}/hash.h
  ${HEADER_PATH}/debugdrawinterface.h
  ${HEADER_PATH}/defaultentitysystem.h
  ${HEADER_PATH}/dynamicscomponent.h
  ${HEADER_PATH}/dynamicproperty.h
  ${HEADER_PATH}/entity.h
  ${HEADER_PATH}/entityid.h
  ${HEADER_PATH}/entitymanager.h
  ${HEADER_PATH}/entitysystem.h
  ${HEADER_PATH}/export.h
  ${HEADER_PATH}/FastDelegate.h
  ${HEADER_PATH}/FastDelegateBind.h
  ${HEADER_PATH}/groupcomponent.h
  ${HEADER_PATH}/initosgviewer.h
  ${HEADER_PATH}/inputinterface.h
  ${HEADER_PATH}/layerattachpointcomponent.h
  ${HEADER_PATH}/layercomponent.h
  ${HEADER_PATH}/lightcomponent.h
  ${HEADER_PATH}/log.h
  ${HEADER_PATH}/logmanager.h
  ${HEADER_PATH}/mapcomponent.h
  ${HEADER_PATH}/mapencoder.h
  ${HEADER_PATH}/matrixtransformcomponent.h
  ${HEADER_PATH}/message.h
  ${HEADER_PATH}/messagefactory.h
  ${HEADER_PATH}/messagepump.h
  ${HEADER_PATH}/nodecomponent.h
  ${HEADER_PATH}/nodemasks.h
  ${HEADER_PATH}/nodemaskvisitor.h
  ${HEADER_PATH}/objectfactory.h
  ${HEADER_PATH}/osganimationcomponent.h
  ${HEADER_PATH}/osgdebugdrawinterface.h
  ${HEADER_PATH}/osginputinterface.h
  ${HEADER_PATH}/osgsysteminterface.h
  ${HEADER_PATH}/osgwindowinterface.h
  ${HEADER_PATH}/positionattitudetransformcomponent.h
  ${HEADER_PATH}/profile.h
  ${HEADER_PATH}/property.h
  ${HEADER_PATH}/propertycontainer.h
  ${HEADER_PATH}/rapidxmlmapencoder.h
  ${HEADER_PATH}/resourcemanager.h
  ${HEADER_PATH}/scriptaccessor.h
  ${HEADER_PATH}/singleton.h
  ${HEADER_PATH}/spawner.h
  ${HEADER_PATH}/shadercomponent.h
  ${HEADER_PATH}/shadowcomponent.h
  ${HEADER_PATH}/skyboxcomponent.h
  ${HEADER_PATH}/staticmeshcomponent.h
  ${HEADER_PATH}/stringid.h
  ${HEADER_PATH}/systeminterface.h
  ${HEADER_PATH}/systemmessages.h
  ${HEADER_PATH}/textlabelcomponent.h
  ${HEADER_PATH}/texturelabelcomponent.h
  ${HEADER_PATH}/threadsafequeue.h
  ${HEADER_PATH}/transformcomponent.h
  ${HEADER_PATH}/uniqueid.h
  ${HEADER_PATH}/windowinterface.h
  ${DTENTITY_CONFIG_HEADER}

)

SET(LIB_REPLACE
  applicationcomponent.cpp
  cameracomponent.cpp
  commandmessages.cpp
  core.cpp
  dynamicscomponent.cpp
  groupcomponent.cpp
  inputinterface.cpp
  layerattachpointcomponent.cpp
  layercomponent.cpp
  lightcomponent.cpp
  mapcomponent.cpp
  matrixtransformcomponent.cpp
  nodecomponent.cpp
  osganimationcomponent.cpp
  osgdebugdrawinterface.cpp
  osginputinterface.cpp
  osgsysteminterface.cpp
  positionattitudetransformcomponent.cpp
  shadercomponent.cpp
  shadowcomponent.cpp
  staticmeshcomponent.cpp
  skyboxcomponent.cpp
  systemmessages.cpp
  textlabelcomponent.cpp
  texturelabelcomponent.cpp
  transformcomponent.cpp
)

SET(LIB_SOURCES
  cameracomponent.cpp
  componentfactories.cpp
  componentpluginmanager.cpp
  dynamicscomponent.cpp
  entity.cpp
  entitymanager.cpp
  hash.cpp
  initosgviewer.cpp
  inputinterface.cpp
  logmanager.cpp
  messagefactory.cpp
  messagepump.cpp
  osgwindowinterface.cpp
  profile.cpp
  property.cpp
  propertycontainer.cpp
  rapidxmlmapencoder.cpp
  resourcemanager.cpp
  scriptaccessor.cpp
  spawner.cpp
  stringid.cpp
  uniqueid.cpp
)

SET(DTENTITYLIBS      ${OPENSCENEGRAPH_LIBRARIES}
                      ${OPENTHREADS_LIBRARIES}
)


IF(PROTOBUF_FOUND)
  INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/gen)
  LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/protobufmapencoder.h
                  gen/data.pb.h)
  LIST(APPEND LIB_SOURCES		protobufmapencoder.cpp
                gen/data.pb.cc)
  LIST(APPEND DTENTITYLIBS   ${PROTOBUF_LIBRARIES})
ENDIF(PROTOBUF_FOUND)


IF(BUILD_OPENAL)
   LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/audiomanager.h)
   LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/listener.h)
   LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/sound.h)
   LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/soundcomponent.h)
   LIST(APPEND LIB_SOURCES		audiomanager.cpp)
   LIST(APPEND LIB_SOURCES		listener.cpp)
   LIST(APPEND LIB_SOURCES		sound.cpp)
   LIST(APPEND LIB_REPLACE		soundcomponent.cpp)
   LIST(APPEND DTENTITYLIBS   ${OPENAL_LIBRARIES})
   LIST(APPEND DTENTITYLIBS   ${OPENAL_UTIL_LIBRARIES})

   # add OpenAL include dir
   INCLUDE_DIRECTORIES( ${OPENAL_INCLUDE_DIR})
ENDIF(BUILD_OPENAL)



REPLACE_SIDS_IN_SOURCE(LIB_SOURCES_OUT ${LIB_REPLACE})

ADD_LIBRARY(${LIB_NAME} ${DTENTITY_LIBS_DYNAMIC_OR_STATIC}
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
    ${LIB_SOURCES_OUT}
    ${LIB_REPLACE}
)


IF (NOT WIN32 AND NOT APPLE)
   FIND_PACKAGE(UUID REQUIRED)
   LIST(APPEND DTENTITYLIBS ${UUID_LIBRARY})
ENDIF (NOT WIN32 AND NOT APPLE)

TARGET_LINK_LIBRARIES(${LIB_NAME} ${DTENTITYLIBS})

IF (WIN32)
   TARGET_LINK_LIBRARIES(${LIB_NAME} Rpcrt4)
ENDIF(WIN32)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_ENTITY_LIBRARY)
INCLUDE(ModuleInstall OPTIONAL)

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")


INSTALL(FILES FindDtEntity.cmake DESTINATION cmake)
