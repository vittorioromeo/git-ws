cmake_minimum_required(VERSION 3.0)

# Include SSVCmake.
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/../SSVCMake/cmake/modules/"
    "${CMAKE_SOURCE_DIR}/extlibs/SSVCMake/cmake/modules/")

include(SSVCMake)

# Basic project setup.
vrm_cmake_init_project(git-ws)
vrm_cmake_find_extlib(vrm_pp)
vrm_cmake_find_extlib(SSVUtils)

# Other compiler flags.
vrm_cmake_add_common_compiler_flags()

# Glob files.
set(INC_DIR "include")
set(SRC_DIR "src")
include_directories("./")
include_directories("./${INC_DIR}")
SSVCMake_setDefaultGlobs()

# Install.
add_executable(${PROJECT_NAME} ${SRC_LIST})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/_RELEASE/)

if(UNIX)
    install(
        PROGRAMS ${CMAKE_SOURCE_DIR}/_RELEASE/${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/
        PERMISSIONS WORLD_EXECUTE WORLD_READ)
elseif(WIN32)
    install(
        PROGRAMS ${CMAKE_SOURCE_DIR}/_RELEASE/${PROJECT_NAME}.exe
        DESTINATION C:/Git/bin/
        PERMISSIONS WORLD_EXECUTE WORLD_READ)
endif()
