cmake_minimum_required(VERSION 3.27)
project(executables_finder C)

set(CMAKE_C_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(executables_finder main.c
        delayed_log/delayed_log.c)
