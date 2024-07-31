cmake_minimum_required(VERSION 3.11)
include(ExternalProject)

project(framework_tests
  LANGUAGES CXX C
  DESCRIPTION "Test different ML Frameworks with POET Data")

add_executable(framework_tests 
                src/framework_test.cpp 
                src/framework_test_func/keras_Rinside.cpp
                src/framework_test_func/tflite.cpp)

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

# Compile NVIDIA-specific parts with nvc++
add_library(framework_tests_nvhpc OBJECT src/gpu_monitor.cpp)
set_target_properties(framework_tests_nvhpc PROPERTIES
    CXX_COMPILER nvc++
    CMAKE_CUDA_COMPILER nvcc
)

ExternalProject_Add(
  tensorflow
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/tensorflow_src/tensorflow/lite
  CMAKE_ARGS 
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/tensorflow
    -DCMAKE_BUILD_TYPE=Release
    -DTFLITE_ENABLE_XNNPACK=ON
    -DTFLITE_ENABLE_GPU=ON
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release
  INSTALL_COMMAND ${CMAKE_COMMAND} --install . --prefix ${CMAKE_BINARY_DIR}/tensorflow)

# include prebuilt EMA
set(EMA_INSTALL_DIR "/mnt/beegfs/home/straile/ema/ema_install") # TODO: use EMA as submoduel
set(EMA_LIB_FILE "${EMA_INSTALL_DIR}/lib/libEMA.so")

# Add include directories
target_include_directories(framework_tests_nvhpc PRIVATE ${CUDA_INCLUDE_DIRS})
target_include_directories(framework_tests PRIVATE 
                           ${EMA_INSTALL_DIR}/include
                           ${CUDA_INCLUDE_DIRS}
                           ${CMAKE_BINARY_DIR}/tensorflow/include)

target_link_libraries(framework_tests PRIVATE
                      RRuntime 
                      Threads::Threads pthread
                      $<TARGET_OBJECTS:framework_tests_nvhpc>
                      ${NVML_LIBRARY}
                      ${CMAKE_DL_LIBS}
                      ${CUDA_LIBRARIES}
                      ${EMA_LIB_FILE}
                      ${CMAKE_BINARY_DIR}/tensorflow-prefix/src/tensorflow-build/libtensorflow-lite.a)

link_directories(${EMA_INSTALL_DIR}/lib)

# specify the C++ standard
set_target_properties(framework_tests  PROPERTIES
                      CMAKE_CXX_STANDARD 20
                      CMAKE_CXX_STANDARD_REQUIRED True
                      LINK_FLAGS "-Wl,-rpath,${EMA_INSTALL_DIR}/lib")

add_dependencies(framework_tests tensorflow)