cmake_minimum_required(VERSION 3.6)
project(pyvgmstream)

set(BUILD_EXE OFF)

option(BUILD_FB2K OFF)
option(BUILD_CLI OFF)
option(BUILD_V123 OFF)

include_directories(${PROJECT_SOURCE_DIR}/vgmstream)

file(GLOB SRC_FILES
    "${PROJECT_SOURCE_DIR}/src/core/*.h"
    "${PROJECT_SOURCE_DIR}/src/core/*.c"
    "${PROJECT_SOURCE_DIR}/src/core/*.cpp"
)

add_subdirectory(${PROJECT_SOURCE_DIR}/vgmstream)

add_library(libvgm ${SRC_FILES})
target_link_libraries(libvgm libvgmstream)
setup_target(libvgm TRUE)

if(NOT BUILD_EXE)
    message(NOTICE "Build pyd")
    add_subdirectory(${PROJECT_SOURCE_DIR}/pybind11)
    pybind11_add_module(libpyvgmstream ${PROJECT_SOURCE_DIR}/src/vgmstream_python.cpp)
    target_link_libraries(libpyvgmstream PRIVATE libvgm)
    # 非常关键！不然死活找不到DLL
    target_link_libraries(libpyvgmstream PRIVATE -static -lpthread -static-libgcc -static-libstdc++)
else()
    message(NOTICE "Build Exe for test")
    add_executable(test ${PROJECT_SOURCE_DIR}/src/test.cpp)
    target_link_libraries(test PRIVATE libvgm)
endif()