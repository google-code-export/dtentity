SET(LIB_NAME dtEntityRocket)


FIND_PACKAGE(V8 REQUIRED)

IF (WIN32)
ADD_DEFINITIONS(-DNOMINMAX)
ENDIF (WIN32)

INCLUDE_DIRECTORIES( 
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
  ${CMAKE_CURRENT_SOURCE_DIR}/../osgLibRocket/source/osgLibRocket/include/
  ${OSG_INCLUDE_DIRS}
  ${LIBROCKET_INCLUDE_DIR}
  ${V8_INCLUDE_DIR}
)

SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR})
SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/contextwrapper.h
   ${HEADER_PATH}/elementdocumentwrapper.h
   ${HEADER_PATH}/elementwrapper.h
   ${HEADER_PATH}/eventinstancer.h
   ${HEADER_PATH}/eventlistener.h
   ${HEADER_PATH}/eventlistenerinstancer.h
   ${HEADER_PATH}/eventlistenerwrapper.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/hudcomponent.h
   ${HEADER_PATH}/messages.h
   ${HEADER_PATH}/rocketcomponent.h
   ${HEADER_PATH}/rocketsystemwrapper.h
)

SET(LIB_SOURCES
	 ${SOURCE_PATH}/contextwrapper.cpp
	 ${SOURCE_PATH}/elementdocumentwrapper.cpp
	 ${SOURCE_PATH}/elementwrapper.cpp
	 ${SOURCE_PATH}/eventinstancer.cpp
	 ${SOURCE_PATH}/eventlistener.cpp
	 ${SOURCE_PATH}/eventlistenerinstancer.cpp
	 ${SOURCE_PATH}/eventlistenerwrapper.cpp
	 ${SOURCE_PATH}/hudcomponent.cpp
	 ${SOURCE_PATH}/messages.cpp
	 ${SOURCE_PATH}/pluginfactory.cpp
	 ${SOURCE_PATH}/rocketcomponent.cpp	 
	 ${SOURCE_PATH}/rocketsystemwrapper.cpp	 
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)


TARGET_LINK_LIBRARIES(${LIB_NAME}     
               ${OSG_LIBRARIES}
               ${OPENTHREADS_LIBRARIES}
               dtEntity
               dtEntityWrappers
               osgLibRocket
               ${V8_LIBRARIES}
)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_ROCKET_LIBRARY)                     
INCLUDE(ModuleInstall OPTIONAL)

IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES OUTPUT_NAME plugins/${LIB_NAME})
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_OUTPUT_NAME plugins/${LIB_NAME})
ENDIF (MSVC) 
