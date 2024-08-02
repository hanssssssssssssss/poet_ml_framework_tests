colnames <- c("H", "O", "Charge", "Ba", "Cl", "S(6)",
              "Sr", "Barite", "Celestite")

import_data <- function(path, iteration) {
  files <- list.files(path, pattern = "iter_", full.names = TRUE)
  content <- readRDS(files[iteration])
  return(content)
}

get_C <- function(field) {
  field <- field$C[colnames]
  return(field)
}

get_T <- function(field) {
  return(field$T[colnames])
}

get_random_indices <- function(length, percentage) {
  set.seed(42) # all test runs train and validate on the same data 
  return(sample(length, size = length * percentage))
}