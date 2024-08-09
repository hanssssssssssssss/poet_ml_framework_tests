colnames <- c("H", "O", "Charge", "Ba", "Cl", "S(6)",
              "Sr", "Barite", "Celestite")


files <- list.files(path="full/", pattern="*.rds")
i = 0
for (file in files) {
  if (i == 1) {
    break
  }
  x <- readRDS(paste0("full/", file))
  write.csv(x$T[colnames], "temp_T_for_python.csv", row.names=FALSE)
  write.csv(x$C[colnames], "temp_C_for_python.csv", row.names=FALSE)
  i <- i + 1
}