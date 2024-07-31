#include <vector>
#include <iostream>
#include <tensorflow/lite/model_builder.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/core/api/op_resolver.h>
#include <tensorflow/lite/interpreter.h>

using Field = std::vector<std::vector<double>>;

void tflite_setup(std::string path_to_model) {
  std::cout << path_to_model << std::endl;
}

void tflite_train(Field &x, Field &y, Field &x_val, Field &y_val) {
  //train
} 
