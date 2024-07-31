#ifndef FRAMEWORK_TEST_FUNCTIONS_H
#define FRAMEWORK_TEST_FUNCTIONS_H

#include <iostream>
#include <RInside.h>

using Field = std::vector<std::vector<double>>;

void R_keras_setup(std::string path_to_model, RInside &R);
void R_keras_train(Field &x, Field &y, Field &x_val, Field &y_val, RInside &R);

void tflite_setup(std::string path_to_model);
void tflite_train(Field &x, Field &y);

void framework_setup(std::string framework, std::string path_to_model, RInside &R) {
    if (!framework.compare("keras3")) {
        R_keras_setup(path_to_model, R);
    } else if (!framework.compare("tflite")) {
        tflite_setup(path_to_model);
    } else {
    std::cout << "Arguments must be: iterations framework model_size" << std::endl;
    std::cout << framework << " is an invalid framework. Choose between [keras3/tflite]" << std::endl;
  }
}

void framework_train(std::string framework, Field &x, Field &y, Field &x_val, Field &y_val, RInside &R) {
    if (!framework.compare("keras3")) {
       R_keras_train(x, y, x_val, y_val, R);
    } else if (!framework.compare("tflite")) {
        tflite_train(x, y);
    } else {
    std::cout << "Invalid Framework argument" << std::endl;
  }
}
#endif //FRAMEWORK_TEST_FUNCTIONS_H