cmake_minimum_required(VERSION 3.31)
project(wol_sender)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DDEBUG)
endif ()

add_executable(wol_sender main.cpp
        WolSender.cpp
        WolSender.h)

if (WIN32)
    target_link_libraries(wol_sender ws2_32)
endif ()