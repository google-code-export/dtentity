SET(LIB_NAME dtEntity)

IF (WIN32)
ADD_DEFINITIONS(-DNOMINMAX)
ENDIF (WIN32)

FIND_PACKAGE(OpenThreads REQUIRED)
FIND_PACKAGE(ProtoBuf)
FIND_PACKAGE(ENet)

OPTION(DTENTITY_USE_STRINGS_AS_STRINGIDS "Use std::string instead of hashed strings (for debugging)" OFF)


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
  ${HEADER_PATH}/commandmessages.h
  ${HEADER_PATH}/component.h
  ${HEADER_PATH}/componentfactories.h
  ${HEADER_PATH}/componentplugin.h
  ${HEADER_PATH}/componentpluginmanager.h
  ${HEADER_PATH}/core.h
  ${HEADER_PATH}/hash.h
  ${HEADER_PATH}/debugdrawinterface.h
  ${HEADER_PATH}/defaultentitysystem.h
  ${HEADER_PATH}/dynamiclibrary.h
  ${HEADER_PATH}/dynamicscomponent.h
  ${HEADER_PATH}/dynamicproperty.h
  ${HEADER_PATH}/entity.h
  ${HEADER_PATH}/entityid.h
  ${HEADER_PATH}/entitymanager.h
  ${HEADER_PATH}/entitysystem.h
  ${HEADER_PATH}/export.h
  ${HEADER_PATH}/FastDelegate.h
  ${HEADER_PATH}/FastDelegateBind.h
  ${HEADER_PATH}/fileutils.h  
  ${HEADER_PATH}/init.h
  ${HEADER_PATH}/inputinterface.h
  ${HEADER_PATH}/log.h
  ${HEADER_PATH}/logmanager.h
  ${HEADER_PATH}/mapcomponent.h
  ${HEADER_PATH}/mapencoder.h
  ${HEADER_PATH}/message.h
  ${HEADER_PATH}/messagefactory.h
  ${HEADER_PATH}/messagepump.h
  ${HEADER_PATH}/nodemasks.h
  ${HEADER_PATH}/objectfactory.h
  ${HEADER_PATH}/profile.h
  ${HEADER_PATH}/property.h
  ${HEADER_PATH}/propertycontainer.h
  ${HEADER_PATH}/rapidxmlmapencoder.h
  ${HEADER_PATH}/scriptaccessor.h
  ${HEADER_PATH}/singleton.h
  ${HEADER_PATH}/spawner.h
  ${HEADER_PATH}/stringid.h
  ${HEADER_PATH}/systeminterface.h
  ${HEADER_PATH}/systemmessages.h
  ${HEADER_PATH}/threadsafequeue.h
  ${HEADER_PATH}/uniqueid.h
  ${HEADER_PATH}/windowinterface.h
  ${DTENTITY_CONFIG_HEADER}

)


SET(LIB_REPLACE
  commandmessages.cpp
  core.cpp
  dynamicscomponent.cpp
  inputinterface.cpp
  mapcomponent.cpp
  systemmessages.cpp
)

SET(LIB_SOURCES
  componentfactories.cpp
  componentpluginmanager.cpp  
  dynamiclibrary.cpp
  entity.cpp
  entitymanager.cpp
  fileutils.cpp
  hash.cpp
  init.cpp
  inputinterface.cpp
  logmanager.cpp
  messagefactory.cpp
  messagepump.cpp
  profile.cpp
  property.cpp
  propertycontainer.cpp
  rapidxmlmapencoder.cpp
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

SETUP_LINK_LIBRARIES(${LIB_NAME})
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")


INSTALL(FILES FindDtEntity.cmake DESTINATION cmake)
