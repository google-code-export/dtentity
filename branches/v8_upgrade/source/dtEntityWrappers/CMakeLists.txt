SET(LIB_NAME dtEntityWrappers)

FIND_PACKAGE(V8 REQUIRED)

INCLUDE_DIRECTORIES( 
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
  ${OSG_INCLUDE_DIR}
  ${V8_INCLUDE_DIR}
)

SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include/${LIB_NAME})

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
    ${HEADER_PATH}/jsproperty.h
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
	buffer.cpp
	bytestorage.cpp
	componentwrapper.cpp
	debugdrawmanagerwrapper.cpp
	entitymanagerwrapper.cpp
	entitysystemjs.cpp
	entitysystemwrapper.cpp
	file.cpp
	globalfunctions.cpp
	inputhandlerwrapper.cpp	
  jsproperty.cpp
  loggerwrapper.cpp
	mapsystemwrapper.cpp
	propertyconverter.cpp
	screenwrapper.cpp
	v8helpers.cpp
	wrappers.cpp
)

SET(LIB_SOURCES_REPLACE
  messages.cpp
  scriptcomponent.cpp
)


SET(WRAPLIBRARIES dtEntity
   ${V8_LIBRARIES}   
   ${V8_BASE_LIBRARIES}
   ${V8_SNAPSHOT_LIBRARIES}
   ${V8_ICUUC_LIBRARIES}
   ${V8_ICUI18N_LIBRARIES}
)


IF(BUILD_OPENAL)
   INCLUDE_DIRECTORIES(${OPENAL_INCLUDE_DIR})
   LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/soundsystemwrapper.h)
	LIST(APPEND LIB_SOURCES		   soundsystemwrapper.cpp)
ENDIF(BUILD_OPENAL)

IF (MSVC)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../ext/win-iconv/)
   LIST(APPEND LIB_SOURCES ../../ext/win-iconv/win_iconv.c)
ENDIF (MSVC)


REPLACE_SIDS_IN_SOURCE(LIB_SOURCES_OUT ${LIB_SOURCES_REPLACE})

ADD_LIBRARY(${LIB_NAME} ${DTENTITY_LIBS_DYNAMIC_OR_STATIC}
  ${LIB_PUBLIC_HEADERS}
	${LIB_SOURCES}
  ${LIB_SOURCES_OUT}
  ${LIB_SOURCES_REPLACE}
)



IF (MSVC)
  LIST(APPEND WRAPLIBRARIES ws2_32.lib winmm.lib)
ENDIF (MSVC)

IF(BUILD_OPENAL)
  LIST(APPEND WRAPLIBRARIES dtEntityAudio)
ENDIF(BUILD_OPENAL)


TARGET_LINK_LIBRARIES( ${LIB_NAME} ${WRAPLIBRARIES})


SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEFINE_SYMBOL DTENTITY_WRAPPERS_LIBRARY)
SETUP_LINK_LIBRARIES(${LIB_NAME})
INCLUDE(ModuleInstall OPTIONAL)

IF (MSVC)
  ADD_DEFINITIONS(-DNODEFAULTLIB:LIBCMT)
  ADD_DEFINITIONS(-DNODEFAULTLIB:LIBCMTD)
ENDIF (MSVC)

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")


