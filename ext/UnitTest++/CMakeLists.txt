SET(LIB_NAME UnitTestPP)

IF (WIN32)
ADD_DEFINITIONS(-DNOMINMAX)
ENDIF (WIN32)


INCLUDE_DIRECTORIES( 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

SET(LIB_PUBLIC_HEADERS
	${HEADER_PATH}/AssertException.h
	${HEADER_PATH}/CheckMacros.h
	${HEADER_PATH}/Checks.h
	${HEADER_PATH}/Config.h
	${HEADER_PATH}/CurrentTest.h
	${HEADER_PATH}/DeferredTestReporter.h
	${HEADER_PATH}/DeferredTestResult.h
	${HEADER_PATH}/ExecuteTest.h
	${HEADER_PATH}/MemoryOutStream.h
	${HEADER_PATH}/ReportAssert.h
	${HEADER_PATH}/Test.h
	${HEADER_PATH}/TestDetails.h
	${HEADER_PATH}/TestList.h
	${HEADER_PATH}/TestMacros.h
	${HEADER_PATH}/TestReporter.h
	${HEADER_PATH}/TestReporterStdout.h
	${HEADER_PATH}/TestResults.h
	${HEADER_PATH}/TestRunner.h
	${HEADER_PATH}/TestSuite.h
	${HEADER_PATH}/TimeConstraint.h
	${HEADER_PATH}/TimeHelpers.h
	${HEADER_PATH}/UnitTest++.h
	${HEADER_PATH}/XmlTestReporter.h
	   
)

SET(LIB_SOURCES
	${SOURCE_PATH}/AssertException.cpp
	${SOURCE_PATH}/Checks.cpp
	${SOURCE_PATH}/CurrentTest.cpp
	${SOURCE_PATH}/DeferredTestReporter.cpp
	${SOURCE_PATH}/DeferredTestResult.cpp
	${SOURCE_PATH}/MemoryOutStream.cpp
	${SOURCE_PATH}/ReportAssert.cpp
	${SOURCE_PATH}/Test.cpp
	${SOURCE_PATH}/TestDetails.cpp
	${SOURCE_PATH}/TestList.cpp
	${SOURCE_PATH}/TestReporter.cpp
	${SOURCE_PATH}/TestReporterStdout.cpp
	${SOURCE_PATH}/TestResults.cpp
	${SOURCE_PATH}/TestRunner.cpp
	${SOURCE_PATH}/TimeConstraint.cpp
	${SOURCE_PATH}/XmlTestReporter.cpp
)

IF (MSVC)
      LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/Win32/TimeHelpers.h)
      LIST(APPEND LIB_SOURCES ${SOURCE_PATH}/Win32/TimeHelpers.cpp)
ELSE (MSVC)
      LIST(APPEND LIB_PUBLIC_HEADERS ${HEADER_PATH}/Posix/TimeHelpers.h
                                     ${HEADER_PATH}/Posix/SignalTranslator.h)
      LIST(APPEND LIB_SOURCES ${SOURCE_PATH}/Posix/TimeHelpers.cpp
                              ${SOURCE_PATH}/Posix/SignalTranslator.cpp)
ENDIF (MSVC)

ADD_LIBRARY(${LIB_NAME} STATIC
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)

#TARGET_LINK_LIBRARIES(${LIB_NAME} ${DTENTITYLIBS})
                      

IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC_IDE)

