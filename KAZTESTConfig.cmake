FIND_PATH( KAZTEST_INCLUDE_DIRS kaztest/kaztest.h /usr/include /usr/local/include $ENV{INCLUDE} )
FIND_PATH( KAZTEST_EXECUTABLE NAMES kaztest_gen PATHS /usr/bin /usr/local/bin )

IF(KAZTEST_INCLUDE_DIRS)
        MESSAGE(STATUS "Found KazTest include dir: ${KAZTEST_INCLUDE_DIRS}")
ELSE(KAZTEST_INCLUDE_DIRS)
        MESSAGE(STATUS "Could NOT find KazTest headers.")
ENDIF(KAZTEST_INCLUDE_DIRS)

IF(KAZTEST_EXECUTABLE)
        MESSAGE(STATUS "Found KazTest generator: ${KAZTEST_EXECUTABLE}")
ELSE(KAZTEST_EXECUTABLE)
        MESSAGE(STATUS "Could NOT find KazTest generator.")
ENDIF(KAZTEST_EXECUTABLE)

IF(KAZTEST_EXECUTABLE AND KAZTEST_INCLUDE_DIRS)
        SET(KAZTEST_EXECUTABLE ${KAZTEST_EXECUTABLE}/kaztest_gen)
        SET(KAZTEST_FOUND "YES")
ELSE()
        SET(KAZTEST_FOUND "NO")
        IF(KAZTEST_FIND_REQUIRED)
                MESSAGE(FATAL_ERROR "Could not find KazTest. Please install KazTest (http://github.com/kazade/kaztest)")
        ENDIF(KAZTEST_FIND_REQUIRED)
ENDIF()