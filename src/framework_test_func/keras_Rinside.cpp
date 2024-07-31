#include <RInside.h>
#include <iostream>

using Field = std::vector<std::vector<double>>;
void R_keras_setup(std::string path_to_model, RInside &R) {
  R.parseEval("source(\"../src/Rfiles/Rkeras3.R\")");
  R.parseEval("model <- initiate_model_xla(\"" + path_to_model + "\")");
  R.parseEval("print(gpu_info())");
}

void R_keras_train(Field &x, Field &y, Field &val_x, Field &val_y, RInside &R) {
  //train
  R["x"] = x;
  R.parseEval("predictors <- setNames(data.frame(x), colnames)");
  R["y"] = y;
  R.parseEval("targets <- setNames(data.frame(y), colnames)");
  R.parseEval("model <- training_step(model, predictors, targets)");
} 
