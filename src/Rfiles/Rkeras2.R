library(keras)
library(tensorflow)

initiate_model <- function() {
  x <- installed.packages(); x[ is.na(x[,"Priority"]), c("Package", "Version")]
  hidden_layers <- c(512, 512, 1024)
  activation <- "relu"
  loss <- "mean_squared_error"

  input_length <- length(9)
  output_length <- length(9)
  ## Creates a new sequential model from scratch
  model <- keras::keras_model_sequential()

  # Input layer defined by input data shape
  model %>% keras::layer_dense(units = input_length,
                        activation = activation,
                        input_shape = input_length,
                        dtype = "float32")

  for (layer_size in hidden_layers) {
    model %>% keras::layer_dense(units = layer_size,
                          activation = activation,
                          dtype = "float32")
  }

  ## Output data defined by output data shape
  model %>% keras::layer_dense(units = output_length,
                        activation = activation,
                        dtype = "float32")

  model %>% keras::compile(loss = loss,
                           optimizer = "adam")
  
  return(model)
}

gpu_info <- function() {
  return(tf_gpu_configured())
}

prediction_step <- function(model, predictors) {
  prediction <- predict(model, as.matrix(predictors))
  colnames(prediction) <- colnames(predictors)
  return(as.data.frame(prediction))
}

training_step <- function(model, predictor, target) {
  x <- as.matrix(predictor)
  y <- as.matrix(target[colnames(x)])

  model %>% fit(x, y, epochs = 10, verbose = 1)

  return(model)
}
