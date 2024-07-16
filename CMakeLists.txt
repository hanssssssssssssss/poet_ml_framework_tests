cmake_minimum_required(VERSION 3.11)

project(framework_tests
  LANGUAGES CXX C
  DESCRIPTION "Test different ML Frameworks with POET Data")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/Modules/")

# Find packages
find_package(RRuntime REQUIRED)
find_package(Threads REQUIRED)

find_package(CUDA REQUIRED)
find_library(NVML_LIBRARY  
             NAMES nvidia-ml libnvidia-ml
             HINTS ${CUDA_TOOLKIT_ROOT_DIR}/lib64
                   ${CUDA_TOOLKIT_ROOT_DIR}/lib
                   ${CUDA_TOOLKIT_ROOT_DIR}/targets/x86_64-linux/lib
             PATH_SUFFIXES stubs)


add_executable(framework_tests src/framework_test.cpp)

# Compile NVIDIA-specific parts with nvc++
add_library(framework_tests_nvhpc OBJECT src/gpu_monitor.cpp)
set_target_properties(framework_tests_nvhpc PROPERTIES
    CXX_COMPILER nvc++
    CMAKE_CUDA_COMPILER nvcc
)

target_include_directories(framework_tests PRIVATE ${CUDA_INCLUDE_DIRS})
target_include_directories(framework_tests_nvhpc PRIVATE ${CUDA_INCLUDE_DIRS})

target_link_libraries(framework_tests PRIVATE
                      RRuntime 
                      Threads::Threads pthread
                      $<TARGET_OBJECTS:framework_tests_nvhpc>
                      ${NVML_LIBRARY})


# specify the C++ standard
set_target_properties(framework_tests  PROPERTIES
                      CMAKE_CXX_STANDARD 20
                      CMAKE_CXX_STANDARD_REQUIRED True)
