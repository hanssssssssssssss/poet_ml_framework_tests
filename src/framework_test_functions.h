#ifndef FRAMEWORK_TEST_FUNCTIONS_H
#define FRAMEWORK_TEST_FUNCTIONS_H

#include <iostream>
#include <RInside.h>

using Field = std::vector<std::vector<double>>;

void R_keras_setup(std::string path_to_model, RInside R);
void R_keras_train(Field x, Field y, RInside R);

#endif //FRAMEWORK_TEST_FUNCTIONS_H