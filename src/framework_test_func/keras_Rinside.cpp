#include <RInside.h>
#include <iostream>

using Field = std::vector<std::vector<double>>;
void R_keras_setup(std::string model, RInside &R) {
  R.parseEval("source(\"../src/Rfiles/Rkeras3.R\")");
  R.parseEval("model <- initiate_model_xla(\"" + model + "\")");
  R.parseEval("print(gpu_info())");
}

void R_keras_train(Field &x, Field &y, Field &x_val, Field &y_val, int batch_size,
                   RInside &R, std::string pid) {
  //train
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
  R.parseEval("saveRDS(history, file = \"output.training_history." + pid + ".rds\")");
}
