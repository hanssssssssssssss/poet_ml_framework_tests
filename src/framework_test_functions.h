#ifndef FRAMEWORK_TEST_FUNCTIONS_H
#define FRAMEWORK_TEST_FUNCTIONS_H

#include <iostream>
#include <RInside.h>
#include <tensorflow/lite/model_builder.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/core/api/op_resolver.h>
#include <tensorflow/lite/interpreter.h>

using Field = std::vector<std::vector<double>>;

void R_keras_setup(std::string model, RInside &R);
void R_keras_train(Field &x, Field &y, Field &x_val, Field &y_val, int batch_size, 
                   RInside &R, std::string pid);

void R_keras_predict(Field &x, int batch_size, RInside &R);

std::unique_ptr<tflite::FlatBufferModel> tflite_setup(std::string path_to_model);
void tflite_train(Field &x, Field &y, Field &x_val, Field &y_val, int batch_size, std::string pid);

void framework_setup(std::string framework, std::string model, RInside &R) {
  if (!framework.compare("keras3")) {
      R_keras_setup(model, R);
  } else if (!framework.compare("tflite")) {
      tflite_setup(model);
  } else {
    std::cout << "Arguments must be: framework task model_size batch_size" << std::endl;
    std::cout << framework << " is an invalid framework. Choose between [keras3/tflite]" << std::endl;
  }
}

void framework_train(std::string framework, Field &x, Field &y, Field &x_val, Field &y_val, 
                     int batch_size, RInside &R, std::string pid) {

  if (!framework.compare("keras3")) {
      R_keras_train(x, y, x_val, y_val, batch_size, R, pid);
  } else if (!framework.compare("tflite")) {
      tflite_train(x, y, x_val, y_val, batch_size, pid);
  } else {
    std::cout << "Arguments must be: framework task model_size batch_size" << std::endl;
    std::cout << framework << " is an invalid framework. Choose between [keras3/tflite]" << std::endl;
  }
}

void framework_predict(std::string framework, Field &x, int batch_size, RInside &R) {
  if (!framework.compare("keras3")) {
    R_keras_predict(x, batch_size, R);
  } else if (!framework.compare("tflite")) {
    // TODO
  } else {
    std::cout << "Arguments must be: framework task model_size batch_size" << std::endl;
    std::cout << framework << " is an invalid framework. Choose between [keras3/tflite]" << std::endl;
  }
}
#endif //FRAMEWORK_TEST_FUNCTIONS_H