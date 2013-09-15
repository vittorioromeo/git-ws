cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type.")
project(git-ws)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -W -pedantic -pthread" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-fno-omit-frame-pointer -g3 -gdwarf-2 -O0" CACHE STRING "" FORCE)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/;${CMAKE_MODULE_PATH}")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/extlibs/SSVJsonCpp/cmake/modules/;${CMAKE_MODULE_PATH}")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/extlibs/SSVUtils/cmake/modules/;${CMAKE_MODULE_PATH}")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/extlibs/SSVUtilsJson/cmake/modules/;${CMAKE_MODULE_PATH}")

find_package(SSVJsonCpp REQUIRED)
find_package(SSVUtils REQUIRED)
find_package(SSVUtilsJson REQUIRED)

set(SRC_DIR "src")
set(INC_DIR "include")
file(GLOB_RECURSE SRC_LIST "${INC_DIR}/*" "${SRC_DIR}/*")

include_directories(${INC_DIR})
include_directories(${SSVJSONCPP_INCLUDE_DIR})
include_directories(${SSVUTILS_INCLUDE_DIR})
include_directories(${SSVUTILSJSON_INCLUDE_DIR})
add_executable(${PROJECT_NAME} ${SRC_LIST})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/_RELEASE/)

if(UNIX)
	install(PROGRAMS ${CMAKE_SOURCE_DIR}/_RELEASE/git-ws DESTINATION /usr/local/bin/ PERMISSIONS WORLD_EXECUTE WORLD_READ)
endif()
