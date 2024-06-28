import_data <- function(path, iteration) {
  colnames <- c("H", "O", "Charge", "Ba", "Cl", "S(6)",
                "Sr", "Barite", "Celestite")

  files <- list.files(path, pattern = "iter_", full.names = TRUE)

  content <- readRDS(files[iteration])
  return(content$C[colnames])
}