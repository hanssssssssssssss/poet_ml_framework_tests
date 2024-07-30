#include <vector>
#include <iostream>

using Field = std::vector<std::vector<double>>;

void tflite_setup(std::string path_to_model) {
  std::cout << path_to_model << std::endl;
}

void tflite_train(Field &x, Field &y) {
  //train
  std::cout << "HALOOOOO 2" << std::endl;
} 
