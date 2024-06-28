# prepare R environment (Rcpp + RInside)
find_program(R_EXE "R")

# search for R executable, R header file and library path
if(R_EXE)
  execute_process(
    COMMAND ${R_EXE} RHOME
    OUTPUT_VARIABLE R_ROOT_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  find_path(
    R_INCLUDE_DIR R.h
    HINTS ${R_ROOT_DIR}
    PATHS /usr/include /usr/local/include /usr/share
    PATH_SUFFIXES include/R R/include
  )

  find_library(
    R_LIBRARY libR.so
    HINTS ${R_ROOT_DIR}/lib
  )
else()
  message(FATAL_ERROR "No R runtime found!")
endif()

mark_as_advanced(R_INCLUDE_DIR R_LIBRARY R_EXE)

set(R_LIBRARIES ${R_LIBRARY})
set(R_INCLUDE_DIRS ${R_INCLUDE_DIR})

# find Rcpp include directory
execute_process(COMMAND Rscript -e "cat(system.file(package='Rcpp'))"
  RESULT_VARIABLE RCPP_NOT_FOUND
  ERROR_QUIET
  OUTPUT_VARIABLE RCPP_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(RCPP_NOT_FOUND)
  message(FATAL_ERROR "Rcpp not found!")
endif()

find_path(R_Rcpp_INCLUDE_DIR Rcpp.h
  HINTS ${RCPP_PATH}
  PATH_SUFFIXES include)

mark_as_advanced(R_Rcpp_INCLUDE_DIR)

list(APPEND R_INCLUDE_DIRS ${R_Rcpp_INCLUDE_DIR})

# find RInside libraries and include path
execute_process(COMMAND Rscript -e "cat(system.file(package='RInside'))"
  RESULT_VARIABLE RINSIDE_NOT_FOUND
  ERROR_QUIET
  OUTPUT_VARIABLE RINSIDE_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(RInside_NOT_FOUND)
  message(FATAL_ERROR "RInside not found!")
endif()

find_library(R_RInside_LIBRARY libRInside.so
  HINTS ${RINSIDE_PATH}/lib)


find_path(R_RInside_INCLUDE_DIR RInside.h
  HINTS ${RINSIDE_PATH}
  PATH_SUFFIXES include)

list(APPEND R_LIBRARIES ${R_RInside_LIBRARY})
list(APPEND R_INCLUDE_DIRS ${R_RInside_INCLUDE_DIR})

mark_as_advanced(R_RInside_LIBRARY R_RInside_INCLUDE_DIR)

# putting all together into interface library

add_library(RRuntime INTERFACE IMPORTED)
set_target_properties(
  RRuntime PROPERTIES
  INTERFACE_LINK_LIBRARIES "${R_LIBRARIES}"
  INTERFACE_INCLUDE_DIRECTORIES "${R_INCLUDE_DIRS}"
)

unset(R_LIBRARIES)
unset(R_INCLUDE_DIRS)
