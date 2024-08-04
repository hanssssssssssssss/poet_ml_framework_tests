require(keras3)
require(tensorflow)

set.seed(42)
tf$random$set_seed(42)
reticulate::py_set_seed(42)

initiate_model_xla <- function(model) {
  Sys.setenv(TF_XLA_FLAGS = "--tf_xla_cpu_global_jit")
  Sys.setenv(XLA_FLAGS = "--xla_gpu_cuda_data_dir=/mnt/beegfs/apps/cuda/12.0")

  loss <- loss_huber() # TODO: fixed for now, we'll care about this later

  if (model == "small") {
    model <- initiate_model(c(128, 128), loss)
  } else if (model == "large") {
    model <- initiate_model(c(512, 1024, 512), loss)
  } else if (grepl(".keras", model)) {
    model <- keras3::load_model(model)
  } else {
    stop(paste(model, "is an invalid framework. Choose between [small/large] or supply the path to a '.keras' file"))
  }
  return(model)
}

gpu_info <- function() {
  return(tf_gpu_configured())
}

prediction_step <- function(model, predictors, batch_size) {
  prediction <- model %>% predict(as.matrix(predictors), batch_size = batch_size)
  colnames(prediction) <- colnames(predictors)
  return(as.data.frame(prediction))
}

training_step <- function(model, x, y, x_val, y_val, batch_size) {
  epochs <- 2000 # This is a constant parameter during all experiments
  x <- as.matrix(x)
  y <- as.matrix(y[colnames(x)])
  x_val <- as.matrix(x_val)
  y_val <- as.matrix(y_val[colnames(x)])

  history <- model %>% fit(x, y, epochs = epochs, batch_size = batch_size,
                           validation_data = list(x_val, y_val))
  return(history)
}
