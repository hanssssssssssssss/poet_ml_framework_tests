cmake_minimum_required(VERSION 3.16)
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

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/src/config.h
)

if(USE_GPU)
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
  target_include_directories(framework_tests_nvhpc PRIVATE ${CUDA_INCLUDE_DIRS})
  target_link_libraries(framework_tests PRIVATE $<TARGET_OBJECTS:framework_tests_nvhpc>)
endif()

# include prebuilt EMA
option(USE_GPU "Use GPU acceleration" OFF)
# Set EMA_INSTALL_DIR based on USE_GPU
if(USE_GPU)
  message("Configuring EMA with GPU")
  set(EMA_INSTALL_DIR "/mnt/beegfs/home/straile/ema/ema_install")
  set(ENV{LD_LIBRARY_PATH} "$ENV{LD_LIBRARY_PATH}:/mnt/beegfs/home/straile/ema/ema_install/libs")
else()
  message("Configuring EMA without GPU")
  set(EMA_INSTALL_DIR "/mnt/beegfs/home/straile/ema_no_gpu/ema_install")
  set(ENV{LD_LIBRARY_PATH} "$ENV{LD_LIBRARY_PATH}:/mnt/beegfs/home/straile/ema_no_gpu/ema_install/libs")
endif()
set(EMA_LIB_FILE "${EMA_INSTALL_DIR}/lib/libEMA.so")

# include prebuilt tensorflow
set(TENSORFLOW_INSTALL_DIR "/mnt/beegfs/home/straile/tflite_build") # TODO: use Tensorflow as submodule?
set(TENSORFLOW_LIB_FILE "${TENSORFLOW_INSTALL_DIR}/libtensorflow-lite.a")

# Add include directories
target_include_directories(framework_tests PRIVATE 
                           ${EMA_INSTALL_DIR}/include
                           ${TENSORFLOW_INSTALL_DIR}/include
                           ${CUDA_INCLUDE_DIRS})

target_link_libraries(framework_tests PRIVATE
                      RRuntime 
                      Threads::Threads pthread
                      ${NVML_LIBRARY}
                      ${CMAKE_DL_LIBS}
                      ${CUDA_LIBRARIES}
                      ${EMA_LIB_FILE}
                      ${TENSORFLOW_LIB_FILE})

link_directories(${EMA_INSTALL_DIR}/lib)

# specify the C++ standard
set_target_properties(framework_tests PROPERTIES
                      CMAKE_CXX_STANDARD 20
                      CMAKE_CXX_STANDARD_REQUIRED True
                      LINK_FLAGS "-Wl,-rpath,${EMA_INSTALL_DIR}/lib")          