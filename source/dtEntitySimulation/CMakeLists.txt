SET(LIB_NAME dtEntitySimulation)

IF (WIN32)
ADD_DEFINITIONS(-DNOMINMAX)
ENDIF (WIN32)

INCLUDE_DIRECTORIES( 
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
  ${OSG_INCLUDE_DIRS}
)

SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/groundclampingcomponent.h
   ${HEADER_PATH}/messages.h
   ${HEADER_PATH}/manipulatorcomponent.h
   ${HEADER_PATH}/manipulators.h
   ${HEADER_PATH}/particlecomponent.h
   ${HEADER_PATH}/pathcomponent.h
   ${HEADER_PATH}/quattools.h
)

SET(LIB_SOURCES
	 ${SOURCE_PATH}/groundclampingcomponent.cpp
	 ${SOURCE_PATH}/messages.cpp
	 ${SOURCE_PATH}/manipulatorcomponent.cpp
	 ${SOURCE_PATH}/manipulators.cpp
	 ${SOURCE_PATH}/particlecomponent.cpp
	 ${SOURCE_PATH}/pathcomponent.cpp
	 ${SOURCE_PATH}/pluginfactory.cpp
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)


TARGET_LINK_LIBRARIES(${LIB_NAME}     
               ${OSG_LIBRARIES}
               ${OPENTHREADS_LIBRARIES}
               dtEntity
)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_ENTITY_SIMULATION_LIBRARY)                     
INCLUDE(ModuleInstall OPTIONAL)

IF (MSVC)
   install(TARGETS ${LIB_NAME}
      ARCHIVE
      DESTINATION lib
      COMPONENT libs
   )
   install(TARGETS ${LIB_NAME}
      RUNTIME
      DESTINATION bin/plugins
      COMPONENT application
   )
ENDIF (MSVC)

IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES OUTPUT_NAME plugins/${LIB_NAME})
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_OUTPUT_NAME plugins/${LIB_NAME})
ENDIF (MSVC) 
