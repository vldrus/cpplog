# SPDX-License-Identifier: MIT #

cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(cpplog VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cpplog INTERFACE)
add_library(cpplog::cpplog ALIAS cpplog)

set_target_properties(cpplog PROPERTIES PUBLIC_HEADER cpplog.h)
target_include_directories(cpplog INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS cpplog)

add_executable(cpplog_test EXCLUDE_FROM_ALL cpplog_test.cc)
target_link_libraries(cpplog_test PRIVATE cpplog::cpplog)
