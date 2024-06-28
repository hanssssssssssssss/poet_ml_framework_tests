cmake_minimum_required(VERSION 3.14)

project(framework_tests
  LANGUAGES CXX C
  DESCRIPTION "Test different ML Frameworks with POET Data")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/Modules/")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(RRuntime REQUIRED)

add_executable(framework_tests src/framework_test.cpp)

target_link_libraries(framework_tests RRuntime)
