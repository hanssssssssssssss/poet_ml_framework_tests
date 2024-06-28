library(keras)
library(tensorflow)

initiate_model <- function(path) {
  model <- keras3::load_model(path)
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
