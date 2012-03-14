# INSTALL and SOURCE_GROUP commands for dtEntity Plugins
#
# Required Vars:
# ${TARGET_NAME}
# ${LIB_PUBLIC_HEADERS}

SET(INSTALL_INCDIR include)
SET(INSTALL_BINDIR plugins)
IF(WIN32)
    SET(INSTALL_LIBDIR plugins)
    SET(INSTALL_ARCHIVEDIR lib)
ELSE(WIN32)
    SET(INSTALL_LIBDIR plugins)
    SET(INSTALL_ARCHIVEDIR lib${LIB_POSTFIX})
ENDIF(WIN32)

SET(HEADERS_GROUP "Header Files")

SOURCE_GROUP(
    ${HEADERS_GROUP}
    FILES ${LIB_PUBLIC_HEADERS}
)

INSTALL(
    TARGETS ${TARGET_NAME} ${APP_NAME}
    RUNTIME DESTINATION ${INSTALL_BINDIR}
    LIBRARY DESTINATION ${INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${INSTALL_ARCHIVEDIR}
)
