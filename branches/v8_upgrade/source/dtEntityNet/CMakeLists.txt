SET(LIB_NAME dtEntityNet)

FIND_PACKAGE(ENet REQUIRED)
FIND_PACKAGE(ProtoBuf REQUIRED)

INCLUDE_DIRECTORIES(${ENET_INCLUDE_DIR})

SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/deadreckoning.h
  ${HEADER_PATH}/deadreckoningreceivercomponent.h
  ${HEADER_PATH}/deadreckoningsendercomponent.h
  ${HEADER_PATH}/enetcomponent.h
  ${HEADER_PATH}/export.h
  ${HEADER_PATH}/messages.h 
)

SET(LIB_SOURCES
  deadreckoning.cpp
  deadreckoningreceivercomponent.cpp
  deadreckoningsendercomponent.cpp
  enetcomponent.cpp
  messages.cpp  
)

SET(LIB_SOURCES_REPLACE
  deadreckoningreceivercomponent.cpp
  deadreckoningsendercomponent.cpp
  enetcomponent.cpp
)


SET(WRAPLIBRARIES dtEntity dtEntityOSG ${ENET_LIBRARIES})

IF (WIN32)
  LIST(APPEND WRAPLIBRARIES ws2_32.lib winmm.lib)
ENDIF (WIN32)

REPLACE_SIDS_IN_SOURCE(LIB_SOURCES_OUT ${LIB_SOURCES_REPLACE})

ADD_LIBRARY(${LIB_NAME} ${DTENTITY_LIBS_DYNAMIC_OR_STATIC}
  ${LIB_PUBLIC_HEADERS}
  ${LIB_SOURCES}
  ${LIB_SOURCES_OUT}
  ${LIB_SOURCES_REPLACE}
)

TARGET_LINK_LIBRARIES( ${LIB_NAME} ${WRAPLIBRARIES})


SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DTENTITY_NET_LIBRARY)
INCLUDE(ModuleInstall OPTIONAL)

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")


