CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(KazTest)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/kaztest/kaztest.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/kaztest)
INSTALL(
    FILES ${CMAKE_SOURCE_DIR}/bin/kaztest_gen DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    PERMISSIONS OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/KAZTESTConfig.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kaztest)
