#include <vector>
#include <iostream>
#include <tensorflow/lite/model_builder.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/core/api/op_resolver.h>
#include <tensorflow/lite/interpreter.h>

using Field = std::vector<std::vector<double>>;

std::unique_ptr<tflite::FlatBufferModel> tflite_setup(std::string path_to_model) {
  std::cout << path_to_model << std::endl;
  tflite::FlatBufferModel::BuildFromFile(path_to_model);

  // Build the interpreter
  tflite::ops::builtin::BuiltinOpResolver resolver;
  std::unique_ptr<tflite::Interpreter> interpreter;
  tflite::InterpreterBuilder(*model, resolver)(&interpreter);

  // Allocate tensor buffers.
  interpreter->AllocateTensors();

  // Get input and output tensors.
  float* input = interpreter->typed_input_tensor<float>(0);
  float* output = interpreter->typed_output_tensor<float>(0);

  return model;
}

void tflite_train(Field &x, Field &y, Field &x_val, Field &y_val, int batch_size, std::string pid) {
  std::cout << "HALOOO" << std::endl;
} 


std::vector<float> tflite_predict(const std::vector<float>& input_data) {
    // Copy input data
    std::copy(input_data.begin(), input_data.end(), input);

    // Run inference
    interpreter->Invoke();

    // Get output (Assuming output size is 9)
    std::vector<float> output_data(output, output + 9);
    return output_data;
} 