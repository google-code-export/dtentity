SET(LIB_NAME dtEntityWrappers)

FIND_PACKAGE(V8 REQUIRED)


IF(BUILD_CEGUI)
	FIND_PACKAGE(CEGUI REQUIRED)
ENDIF(BUILD_CEGUI)

IF(BUILD_WITH_DELTA3D)
  IF(BUILD_CAL3D)
    FIND_PACKAGE(CAL3D REQUIRED)
  ENDIF(BUILD_CAL3D)
  OPTION(BUILD_OPENAL "Build OpenAL Sound module" OFF)
ENDIF(BUILD_WITH_DELTA3D)

INCLUDE_DIRECTORIES( 
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
  ${OSG_INCLUDE_DIRS}
  ${V8_INCLUDE_DIR}
  ${CEGUI_INCLUDE_DIR}
  ${DELTA3D_INCLUDE_DIR}
)

SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

SET(LIB_PUBLIC_HEADERS
	${HEADER_PATH}/buffer.h
	${HEADER_PATH}/bytestorage.h
	${HEADER_PATH}/componentwrapper.h
	${HEADER_PATH}/debugdrawmanagerwrapper.h
	${HEADER_PATH}/entitymanagerwrapper.h
	${HEADER_PATH}/entitysystemjs.h
	${HEADER_PATH}/entitysystemwrapper.h
	${HEADER_PATH}/export.h
	${HEADER_PATH}/file.h
	${HEADER_PATH}/globalfunctions.h
	${HEADER_PATH}/inputhandlerwrapper.h
	${HEADER_PATH}/loggerwrapper.h
	${HEADER_PATH}/mapsystemwrapper.h
	${HEADER_PATH}/messages.h
	${HEADER_PATH}/propertyconverter.h
	${HEADER_PATH}/screenwrapper.h
	${HEADER_PATH}/scriptcomponent.h
	${HEADER_PATH}/v8helpers.h
	${HEADER_PATH}/wrappers.h
)

SET(LIB_SOURCES
	${SOURCE_PATH}/buffer.cpp
	${SOURCE_PATH}/bytestorage.cpp
	${SOURCE_PATH}/componentwrapper.cpp
	${SOURCE_PATH}/debugdrawmanagerwrapper.cpp
	${SOURCE_PATH}/entitymanagerwrapper.cpp
	${SOURCE_PATH}/entitysystemjs.cpp
	${SOURCE_PATH}/entitysystemwrapper.cpp
	${SOURCE_PATH}/file.cpp
	${SOURCE_PATH}/globalfunctions.cpp
	${SOURCE_PATH}/inputhandlerwrapper.cpp	
    ${SOURCE_PATH}/loggerwrapper.cpp
	${SOURCE_PATH}/mapsystemwrapper.cpp
	${SOURCE_PATH}/messages.cpp
	${SOURCE_PATH}/propertyconverter.cpp
	${SOURCE_PATH}/screenwrapper.cpp
	${SOURCE_PATH}/scriptcomponent.cpp	
	${SOURCE_PATH}/v8helpers.cpp
	${SOURCE_PATH}/wrappers.cpp
)

SET(WRAPLIBRARIES dtEntity
                  ${V8_LIBRARIES}
                  ${OSG_LIBRARIES}
                  ${OPENTHREADS_LIBRARIES}
)

IF(BUILD_CEGUI)
	LIST(APPEND WRAPLIBRARIES ${CEGUI_LIBRARIES})
	LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/guiwidgetwrapper.h
								   ${HEADER_PATH}/guiwrapper.h)
	LIST(APPEND LIB_SOURCES		   ${SOURCE_PATH}/guiwidgetwrapper.cpp
								   ${SOURCE_PATH}/guiwrapper.cpp)
	
ENDIF(BUILD_CEGUI)

IF(BUILD_CAL3D)
	LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/animationsystemwrapper.h)
	LIST(APPEND LIB_SOURCES		   ${SOURCE_PATH}/animationsystemwrapper.cpp)
	LIST(APPEND WRAPLIBRARIES      ${DTANIM_LIBRARIES}
	)
ENDIF(BUILD_CAL3D)

IF(BUILD_OPENAL)
    LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/soundsystemwrapper.h)
	LIST(APPEND LIB_SOURCES		   ${SOURCE_PATH}/soundsystemwrapper.cpp)
ENDIF(BUILD_OPENAL)

IF (MSVC)
  INCLUDE_DIRECTORIES( 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/win-iconv/
   )
   LIST(APPEND LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/win-iconv/win_iconv.c)
ENDIF (MSVC)

ADD_LIBRARY(${LIB_NAME} ${DTENTITY_LIBS_DYNAMIC_OR_STATIC}
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)



IF (MSVC)
  LIST(APPEND WRAPLIBRARIES ws2_32.lib winmm.lib)
ENDIF (MSVC)

TARGET_LINK_LIBRARIES( ${LIB_NAME} ${WRAPLIBRARIES})


SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DTENTITY_WRAPPERS_LIBRARY)
INCLUDE(ModuleInstall OPTIONAL)

IF (MSVC)
  ADD_DEFINITIONS(-DNODEFAULTLIB:LIBCMT)
  ADD_DEFINITIONS(-DNODEFAULTLIB:LIBCMTD)
ENDIF (MSVC)

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")


