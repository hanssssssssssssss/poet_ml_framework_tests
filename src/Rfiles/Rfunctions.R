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
  #set.seed(42)   # Setting the random seed breaks keras!
  indices <- sample(length, size = length * percentage)
  #set.seed(NULL)
  return(indices)
}

get_indices_from_file <- function(path) {
  # To make training repeatable load predetermined indices
  return(readRDS(path))
}

save_training_history <- function(history, pid) {
  val_loss <- history$metrics$val_loss
  write.table(t(val_loss), sep = ",",
              file = paste0("output.training_history.", pid, ".csv"),
              row.names = FALSE, col.names = FALSE)
}