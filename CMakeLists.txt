cmake_minimum_required(VERSION 3.13)

project(
  DiskAnalyzer
  LANGUAGES CXX
  VERSION 0.1)

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

include(cmake/CompilerWarnings.cmake)
include(cmake/Coverage.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/StandardProjectSettings.cmake)

add_library(project_options INTERFACE)
add_library(project::options ALIAS project_options)
target_compile_features(project_options INTERFACE cxx_std_17)

add_library(project_warnings INTERFACE)
add_library(project::warnings ALIAS project_warnings)

set_project_warnings(project_warnings)
enable_sanitizers(project_options)
enable_coverage(project_options)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/shared/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/client/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/server/)

option(BUILD_TESTS "Build the tests that come with `da`" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests/)
endif()
