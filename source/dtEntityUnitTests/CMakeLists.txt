SET(APP_NAME dtEntityUnitTests)

INCLUDE_DIRECTORIES( 
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
  ${OSG_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/UnitTest++/src
)

SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR})
SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})


SET(LIB_SOURCES
	 ${SOURCE_PATH}/testDynamicProperties.cpp
	 ${SOURCE_PATH}/testEntityManager.cpp
	 ${SOURCE_PATH}/testInitOsgViewer.cpp
	 ${SOURCE_PATH}/testMap.cpp
	 ${SOURCE_PATH}/testProperties.cpp
	 ${SOURCE_PATH}/testPropertyContainer.cpp
	 ${SOURCE_PATH}/testScene.cpp
	 ${SOURCE_PATH}/testScriptAccessor.cpp
	 ${SOURCE_PATH}/testSpawner.cpp
)

ADD_EXECUTABLE(${APP_NAME}
    ${LIB_SOURCES}
)


TARGET_LINK_LIBRARIES(${APP_NAME}     
               ${OPENSCENEGRAPH_LIBRARIES}
               ${OPENTHREADS_LIBRARIES}
               dtEntity
               UnitTestPP               
)

SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEFINE_SYMBOL DT_ENTITY_UNITTEST_LIBRARY)                     

SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

