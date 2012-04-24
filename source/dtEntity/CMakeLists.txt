SET(LIB_NAME dtEntity)

IF (WIN32)
ADD_DEFINITIONS(-DNOMINMAX)
ENDIF (WIN32)

FIND_PACKAGE(OpenThreads REQUIRED)

OPTION(DTENTITY_USE_STRINGS_AS_STRINGIDS "Use std::string instead of hashed strings (for debugging)" OFF)

# link against delta3d libs?
OPTION(BUILD_WITH_DELTA3D "Build with Delta3D" OFF)
IF(BUILD_WITH_DELTA3D)
  FIND_PACKAGE(Delta3D REQUIRED)

  LINK_DIRECTORIES( ${DELTA3D_LIB_DIR} )
  INCLUDE_DIRECTORIES(
    ${DELTA3D_INCLUDE_DIR}
  )
  IF (DELTA3D_EXT_DIR)
    LINK_DIRECTORIES( ${DELTA3D_EXT_DIR}/lib )
  ENDIF (DELTA3D_EXT_DIR)
  OPTION(BUILD_CAL3D "Make Cal3D accessible from JavaScript" OFF)
  IF(BUILD_CAL3D)
    FIND_PACKAGE(CAL3D REQUIRED)
  ENDIF(BUILD_CAL3D)

ELSE(BUILD_WITH_DELTA3D)
  SET(BUILD_CAL3D OFF)
ENDIF(BUILD_WITH_DELTA3D)

# Flag to build SoundSystem stuff (requires OpenAL)
OPTION(BUILD_OPENAL "Build OpenAL Sound module" OFF)
IF(BUILD_OPENAL)
  FIND_PACKAGE(OpenAL REQUIRED)
  INCLUDE_DIRECTORIES(
    ${OPENAL_INC_PATHS}
  )
ENDIF(BUILD_OPENAL)

IF(BUILD_CEGUI)
   FIND_PACKAGE(CEGUI REQUIRED)
   INCLUDE_DIRECTORIES(${CEGUI_INCLUDE_DIR})
ENDIF(BUILD_CEGUI)

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
  ${HEADER_PATH}/basemessages.h
  ${HEADER_PATH}/cameracomponent.h
  ${HEADER_PATH}/commandmessages.h
  ${HEADER_PATH}/component.h
  ${HEADER_PATH}/componentfactories.h
  ${HEADER_PATH}/componentplugin.h
  ${HEADER_PATH}/componentpluginmanager.h
  ${HEADER_PATH}/crc32.h
  ${HEADER_PATH}/debugdrawmanager.h
  ${HEADER_PATH}/defaultentitysystem.h
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
  ${HEADER_PATH}/inputhandler.h
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
  ${HEADER_PATH}/policycomponent.h
  ${HEADER_PATH}/positionattitudetransformcomponent.h
  ${HEADER_PATH}/pickshapecomponent.h
  ${HEADER_PATH}/profile.h
  ${HEADER_PATH}/property.h
  ${HEADER_PATH}/propertycontainer.h
  ${HEADER_PATH}/rapidxmlmapencoder.h
  ${HEADER_PATH}/scriptaccessor.h
  ${HEADER_PATH}/singleton.h
  ${HEADER_PATH}/spawner.h
  ${HEADER_PATH}/shadercomponent.h
  ${HEADER_PATH}/shadowcomponent.h
  ${HEADER_PATH}/skyboxcomponent.h
  ${HEADER_PATH}/staticmeshcomponent.h
  ${HEADER_PATH}/stringid.h
  ${HEADER_PATH}/systemmessages.h
  ${HEADER_PATH}/textlabelcomponent.h
  ${HEADER_PATH}/texturelabelcomponent.h
  ${HEADER_PATH}/threadsafequeue.h
  ${HEADER_PATH}/transformcomponent.h
  ${HEADER_PATH}/windowmanager.h
  ${DTENTITY_CONFIG_HEADER}

)

SET(LIB_SOURCES


  ${SOURCE_PATH}/applicationcomponent.cpp
  ${SOURCE_PATH}/cameracomponent.cpp
  ${SOURCE_PATH}/commandmessages.cpp
  ${SOURCE_PATH}/componentfactories.cpp
  ${SOURCE_PATH}/componentpluginmanager.cpp
  ${SOURCE_PATH}/crc32.cpp
  ${SOURCE_PATH}/debugdrawmanager.cpp
  ${SOURCE_PATH}/entity.cpp
  ${SOURCE_PATH}/entitymanager.cpp
  ${SOURCE_PATH}/groupcomponent.cpp
  ${SOURCE_PATH}/initosgviewer.cpp
  ${SOURCE_PATH}/inputhandler.cpp
  ${SOURCE_PATH}/layerattachpointcomponent.cpp
  ${SOURCE_PATH}/layercomponent.cpp
  ${SOURCE_PATH}/lightcomponent.cpp
  ${SOURCE_PATH}/logmanager.cpp
  ${SOURCE_PATH}/mapcomponent.cpp
  ${SOURCE_PATH}/matrixtransformcomponent.cpp
  ${SOURCE_PATH}/messagefactory.cpp
  ${SOURCE_PATH}/messagepump.cpp
  ${SOURCE_PATH}/nodecomponent.cpp
  ${SOURCE_PATH}/osganimationcomponent.cpp
  ${SOURCE_PATH}/pickshapecomponent.cpp
  ${SOURCE_PATH}/positionattitudetransformcomponent.cpp
  ${SOURCE_PATH}/profile.cpp
  ${SOURCE_PATH}/property.cpp
  ${SOURCE_PATH}/propertycontainer.cpp
  ${SOURCE_PATH}/rapidxmlmapencoder.cpp
  ${SOURCE_PATH}/scriptaccessor.cpp
  ${SOURCE_PATH}/shadercomponent.cpp
  ${SOURCE_PATH}/shadowcomponent.cpp
  ${SOURCE_PATH}/spawner.cpp
  ${SOURCE_PATH}/staticmeshcomponent.cpp
  ${SOURCE_PATH}/skyboxcomponent.cpp
  ${SOURCE_PATH}/stringid.cpp
  ${SOURCE_PATH}/systemmessages.cpp
  ${SOURCE_PATH}/textlabelcomponent.cpp
  ${SOURCE_PATH}/texturelabelcomponent.cpp
  ${SOURCE_PATH}/transformcomponent.cpp
  ${SOURCE_PATH}/windowmanager.cpp

)

SET(DTENTITYLIBS      ${OPENSCENEGRAPH_LIBRARIES}
                      ${OPENTHREADS_LIBRARIES}
)

IF(BUILD_WITH_DELTA3D)
  LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/d3dwindowmanager.h )
  LIST(APPEND LIB_SOURCES		    ${SOURCE_PATH}/d3dwindowmanager.cpp)
  LIST(APPEND DTENTITYLIBS        ${DTABC_LIBRARIES} ${DTCORE_LIBRARIES} ${DTUTIL_LIBRARIES})
ENDIF(BUILD_WITH_DELTA3D)

IF(BUILD)

  LIST(APPEND DTENTITYLIBS       ${DTANIM_LIBRARIES} ${CAL3D_LIBRARIES})
  LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/animationcomponent.h)
  LIST(APPEND LIB_SOURCES		   ${SOURCE_PATH}/animationcomponent.cpp)
ENDIF(BUILD)

IF(BUILD_OPENAL)
   LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/audiomanager.h)
   LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/listener.h)
   LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/sound.h)
   LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/soundcomponent.h)
   LIST(APPEND LIB_SOURCES		${SOURCE_PATH}/audiomanager.cpp)
   LIST(APPEND LIB_SOURCES		${SOURCE_PATH}/listener.cpp)
   LIST(APPEND LIB_SOURCES		${SOURCE_PATH}/sound.cpp)
   LIST(APPEND LIB_SOURCES		${SOURCE_PATH}/soundcomponent.cpp)
   LIST(APPEND DTENTITYLIBS   ${OPENAL_LIBRARIES})
   LIST(APPEND DTENTITYLIBS   ${OPENAL_UTIL_LIBRARIES})

   # add OpenAL include dir
   INCLUDE_DIRECTORIES( ${OPENAL_INCLUDE_DIR})
ENDIF(BUILD_OPENAL)

IF(BUILD_CEGUI)
   LIST(APPEND DTENTITYLIBS ${CEGUI_LIBRARIES})
   LIST(APPEND DTENTITYLIBS ${CEGUI_OPENGLRENDERER_LIBRARIES})
   LIST(APPEND LIB_PUBLIC_HEADERS
      ${HEADER_PATH}/gui.h
      ${HEADER_PATH}/resourceprovider.h
      ${HEADER_PATH}/scriptmodule.h

  )
   LIST(APPEND LIB_SOURCES
      ${SOURCE_PATH}/gui.cpp
      ${SOURCE_PATH}/resourceprovider.cpp
      ${SOURCE_PATH}/scriptmodule.cpp
)

ENDIF(BUILD_CEGUI)


ADD_LIBRARY(${LIB_NAME} ${DTENTITY_LIBS_DYNAMIC_OR_STATIC}
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
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

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")


INSTALL(FILES FindDtEntity.cmake DESTINATION cmake)
