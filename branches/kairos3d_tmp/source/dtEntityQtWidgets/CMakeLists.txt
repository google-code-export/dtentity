SET(LIB_NAME dtEntityQtWidgets)

FIND_PACKAGE(Qt4 REQUIRED)

SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})


set(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/assetselector.h
   ${HEADER_PATH}/datapatheditor.h
   ${HEADER_PATH}/delegatefactory.h
   ${HEADER_PATH}/entitylist.h
   ${HEADER_PATH}/entitytree.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/glwidgetfactory.h
   ${HEADER_PATH}/listdialog.h
   ${HEADER_PATH}/messages.h
   ${HEADER_PATH}/messagestore.h
   ${HEADER_PATH}/osgadapterwidget.h
   ${HEADER_PATH}/osggraphicswindowqt.h
   ${HEADER_PATH}/propertydelegate.h
   ${HEADER_PATH}/propertyeditor.h
   ${HEADER_PATH}/qtguiwindowsystemwrapper.h
   ${HEADER_PATH}/spawnerstore.h
)


SET(LIB_SOURCES
   ${SOURCE_PATH}/assetselector.cpp
   ${SOURCE_PATH}/datapatheditor.cpp
   ${SOURCE_PATH}/datapatheditor.ui
   ${SOURCE_PATH}/delegatefactory.cpp
   ${SOURCE_PATH}/entitylist.cpp
   ${SOURCE_PATH}/entitytree.cpp
   ${SOURCE_PATH}/icons
   ${SOURCE_PATH}/listdialog.cpp
   ${SOURCE_PATH}/messages.cpp
   ${SOURCE_PATH}/messagestore.cpp
   ${SOURCE_PATH}/messagestore.ui
   ${SOURCE_PATH}/osgadapterwidget.cpp
   ${SOURCE_PATH}/osggraphicswindowqt.cpp
   ${SOURCE_PATH}/propertydelegate.cpp
   ${SOURCE_PATH}/propertyeditor.cpp
   ${SOURCE_PATH}/qtguiwindowsystemwrapper.cpp
   ${SOURCE_PATH}/resources.qrc
   ${SOURCE_PATH}/spawnerstore.cpp
   ${SOURCE_PATH}/spawnerstore.ui
)


set(MOC_INPUT
   ${HEADER_PATH}/assetselector.h
   ${HEADER_PATH}/datapatheditor.h
   ${HEADER_PATH}/entitylist.h
   ${HEADER_PATH}/entitytree.h
   ${HEADER_PATH}/listdialog.h
   ${HEADER_PATH}/messagestore.h
   ${HEADER_PATH}/osgadapterwidget.h
   ${HEADER_PATH}/osggraphicswindowqt.h
   ${HEADER_PATH}/propertydelegate.h
   ${HEADER_PATH}/propertyeditor.h
   ${HEADER_PATH}/spawnerstore.h
)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
  ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
  ${CMAKE_SOURCE_DIR}/source/dtEntity/include
  ${QT_INCLUDE_DIR}
  ${OSG_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/RapidXML
)

QT4_ADD_RESOURCES(RCC_SOURCES
    resources.qrc
)


QT4_WRAP_CPP(MOC_SOURCES ${MOC_INPUT})

QT4_WRAP_UI(LIB_SOURCES 
	assetselector.ui
	datapatheditor.ui
	entitytree.ui
	listdialog.ui
	messagestore.ui
	propertyeditor.ui
	spawnerstore.ui
)

ADD_LIBRARY(${LIB_NAME} SHARED 
    ${LIB_PUBLIC_HEADERS}
    ${MOC_SOURCES}
    ${RCC_SOURCES}
    ${LIB_SOURCES}
 )
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEFINE_SYMBOL ENTITYQTWIDGETS_LIBRARY)

IF (MSVC)
   install(TARGETS ${LIB_NAME}
      ARCHIVE
      DESTINATION lib
      COMPONENT libs
   )
   install(TARGETS ${LIB_NAME}
      RUNTIME
      DESTINATION bin
      COMPONENT application
   )
ENDIF (MSVC)

TARGET_LINK_LIBRARIES(${LIB_NAME}   ${QT_QTCORE_LIBRARIES}
				                    ${QT_QTGUI_LIBRARIES}
				                    ${QT_QTSVG_LIBRARIES}
				                    ${QT_QTOPENGL_LIBRARIES}
				                    ${OSG_LIBRARIES}				                   
				                    dtEntity
				                    ${DTCORE_LIBRARIES}    
				                    ${DTABC_LIBRARIES}    
)

IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC) 

