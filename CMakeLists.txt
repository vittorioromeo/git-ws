cmake_minimum_required(VERSION 2.8)
project(git-ws)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/extlibs/SSVCMake/cmake/modules/;${CMAKE_MODULE_PATH}")
include(SSVCMake)

SSVCMake_setDefaults()
SSVCMake_findExtlib(SSVUtils)
SSVCMake_findExtlib(SSVUtilsJson)

add_executable(${PROJECT_NAME} ${SRC_LIST})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/_RELEASE/)

if(UNIX)
	install(PROGRAMS ${CMAKE_SOURCE_DIR}/_RELEASE/${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/ PERMISSIONS WORLD_EXECUTE WORLD_READ)
endif()
