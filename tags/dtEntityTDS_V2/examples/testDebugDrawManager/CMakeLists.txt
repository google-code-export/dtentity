SET(APP_NAME testDebugDrawManager)

IF (WIN32)
ADD_DEFINITIONS(-DNOMINMAX)
ENDIF (WIN32)

INCLUDE_DIRECTORIES( 
  ${CMAKE_SOURCE_DIR}/${INC_DIR}  
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
  ${DELTA3D_INCLUDE_DIR}
  ${OSG_INCLUDE_DIR}
)

SET(APP_SOURCES
    testdebugdrawmanager.cpp
)

ADD_EXECUTABLE(${APP_NAME}
    ${APP_SOURCES}
)


TARGET_LINK_LIBRARIES(${APP_NAME}     
		      dtEntity
		      ${DTUTIL_LIBRARIES}
			  ${DTCORE_LIBRARIES}                                      
			  ${DTABC_LIBRARIES}                                      
                      )
                     
INCLUDE(ModuleInstall OPTIONAL)

SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES IMPORT_PREFIX "../")
SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

