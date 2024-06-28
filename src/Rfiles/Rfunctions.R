colnames <- c("H", "O", "Charge", "Ba", "Cl", "S(6)",
                "Sr", "Barite", "Celestite")

import_data <- function(path, iteration) {
  files <- list.files(path, pattern = "iter_", full.names = TRUE)
  content <- readRDS(files[iteration])
  return(content)
}

get_C <- function(field) {
    return(field$C[colnames])
}

get_T <- function(field) {
    return(field$T[colnames])
}