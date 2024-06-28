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

preprocess <- function(df, backtransform = FALSE, outputs = FALSE) {
  return(df)
}

training_step <- function(model, predictor, target, validity) {
  x <- as.matrix(predictor)
  y <- as.matrix(target[colnames(x)])

  model %>% fit(x, y)
}
