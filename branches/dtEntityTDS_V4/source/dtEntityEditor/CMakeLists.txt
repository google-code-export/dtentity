SET(APP_NAME dtEntityEditor)

IF(UNIX)
  FIND_PACKAGE(X11 REQUIRED)
  INCLUDE_DIRECTORIES(X11_INCLUDE_DIR)
  SET(ADDITIONAL_LIBS ${X11_LIBRARIES})
ENDIF(UNIX)

FIND_PACKAGE(Qt4 REQUIRED)

SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include/${APP_NAME})
SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(APP_PUBLIC_HEADERS
   ${HEADER_PATH}/editorapplication.h
   ${HEADER_PATH}/editormainwindow.h

)

SET(APP_SOURCES
   ${SOURCE_PATH}/editorapplication.cpp
   ${SOURCE_PATH}/editormainwindow.cpp
   ${SOURCE_PATH}/main.cpp
)

set(MOC_INPUT
   ${HEADER_PATH}/editorapplication.h
   ${HEADER_PATH}/editormainwindow.h
)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
  ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
  ${CMAKE_SOURCE_DIR}/source/dtEntity/include
  ${CMAKE_SOURCE_DIR}/source/dtEntityQtWidgets/include
  ${QT_INCLUDE_DIR}
  ${OSG_INCLUDE_DIR}
  ${OPENTHREADS_INCLUDE_DIR}
)

QT4_WRAP_CPP(MOC_SOURCES ${MOC_INPUT})

ADD_EXECUTABLE(${APP_NAME} 
    ${APP_PUBLIC_HEADERS}
    ${APP_SOURCES}
    ${MOC_SOURCES}
    ${RCC_SOURCES}
)

IF(DTENTITY_LIBRARY_STATIC)
    SET(DTENTITY_PLUGINS dtEntitySimulation dtEntityV8Plugin dtEntityRocket)
ENDIF(DTENTITY_LIBRARY_STATIC)

TARGET_LINK_LIBRARIES(${APP_NAME}
                           ${QT_QTCORE_LIBRARIES}
                           ${QT_QTGUI_LIBRARIES}
                           ${QT_QTOPENGL_LIBRARIES}
                           ${OPENSCENEGRAPH_LIBRARIES}
                           ${ADDITIONAL_LIBS}
                           ${OPENTHREADS_LIBRARIES}
                           dtEntity
                           dtEntityOSG
                           dtEntityQtWidgets
                           ${DTENTITY_PLUGINS}
)

SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")


