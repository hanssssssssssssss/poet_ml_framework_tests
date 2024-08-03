#include <RInside.h>
#include <iostream>
#include "config.h"

using Field = std::vector<std::vector<double>>;
void R_keras_setup(std::string model, RInside &R) {
  const std::string rfile_path = RFILES_DIR;
  R.parseEval("source(\"" + rfile_path + "/Rkeras3.R\")");
  R.parseEval("model <- initiate_model_xla(\"" + model + "\")");
  R.parseEval("print(gpu_info())");
}

void R_keras_train(Field &x, Field &y, Field &x_val, Field &y_val, int batch_size,
                   RInside &R, std::string pid) {
  R["x"] = x;
  R.parseEval("x <- setNames(data.frame(x), colnames)");
  R["y"] = y;
  R.parseEval("y <- setNames(data.frame(y), colnames)");
  R["x_val"] = x_val;
  R.parseEval("x_val <- setNames(data.frame(x_val), colnames)");
  R["y_val"] = y_val;
  R.parseEval("y_val <- setNames(data.frame(y_val), colnames)");
  
  R["batch_size"] = batch_size;

  R.parseEval("history <- training_step(model, x, y, x_val, y_val, batch_size)");
  R.parseEval("keras3::save_model(model, filepath = \"output.model." + pid + ".keras\")");
  R.parseEval("save_training_history(history, " + pid + ")");
}

void R_keras_predict(Field &x, int batch_size, RInside &R) {
  R["x"] = x;
  R["batch_size"] = batch_size;
  R.parseEval("x <- setNames(data.frame(x), colnames)");
  R.parseEval("predictions <- prediction_step(model, x, batch_size)");
}
