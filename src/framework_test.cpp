#include <RInside.h>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <fstream>
#include "gpu_monitor.h"
#include "framework_test_functions.h"
extern "C" {
  #include <EMA.h>
}

using Field = std::vector<std::vector<double>>;

std::atomic<float> gpu_utilization(0.0f);
std::atomic<bool> run_gpu_monitoring(true);
std::string keras_model_path;

int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cout << "Wrong number of arguments!" << std::endl;
    std::cout << "Arguments must be iterations framework model_size" << std::endl;
    return 1;
  }

  // Constants
  const int gpu_measure_interval = 1000;
  const int iterations = std::stoi(argv[1]);
  const std::string framework = argv[2];
  const std::string model = argv[3];
  const float validation_data_percentage = .05;

  if (!model.compare("small")) {
    keras_model_path = "../src/barite_50ai_all.keras";
  } else if (!model.compare("large")) {
    keras_model_path = "../src/EMPTY";
  } else {
    std::cout << "Arguments must be: iterations framework model_size" << std::endl;
    std::cout << model << " is an invalid model size. Choose between [small/large]" << std::endl;
  }

  const pid_t pid = getpid();

  std::cout << "Process ID: " << getpid() << std::endl;
  std::cout << "Uses Framework: " << framework << std::endl;

  // Initialize EMA
  int ema_err = EMA_init(NULL);
  if(ema_err) {
    std::cout << "EMA initalization failed" << std::endl;
    return 1;
  }

  EMA_REGION_DECLARE(load_data);
  EMA_REGION_DECLARE(setup);
  EMA_REGION_DECLARE(training);
  EMA_REGION_DEFINE(&load_data, "load_data");
  EMA_REGION_DEFINE(&setup, "setup");
  EMA_REGION_DEFINE(&training, "training");

  RInside R(argc, argv);

  // Load R util functions
  R.parseEval("source(\"../src/Rfiles/Rfunctions.R\")");
  R.parseEval("source(\"../src/Rfiles/Rfunctions_POET.R\")");

  // Load RDS files
  EMA_REGION_BEGIN(load_data);
  for (int i = 0; i < iterations; i++) {
    R.parseEval("R_temp_data <- import_data(\"../RData/\"," + std::to_string(i + 1) + ")");
    if (i == 0) {
      std::cout << i;
      // Initialize field
      R.parseEval("temp_field_C <- get_C(R_temp_data)");
      R.parseEval("temp_field_T <- get_T(R_temp_data)");
      std::cout << i;
    } else {
      //std::cout << i;
      R.parseEval("temp_field_C <- rbind(temp_field_C, get_C(R_temp_data))");
      R.parseEval("temp_field_T <- rbind(temp_field_T, get_T(R_temp_data))");
    }
  }

  // split into training and validation data
  R["validation_data_percentage"] = validation_data_percentage;
  R.parseEval("validation_indices <- get_random_indices(nrow(temp_field_C), validation_data_percentage)");
  
  R.parseEval("x_train <- temp_field_T[-validation_indices,]");
  R.parseEval("y_train <- temp_field_C[-validation_indices,]");
  R.parseEval("x_val <- temp_field_T[validation_indices,]");
  R.parseEval("y_val <- temp_field_C[validation_indices,]");


  Field x = R["x_train"];
  Field y = R["y_train"];
  Field x_val = R["x_val"];
  Field y_val = R["y_val"];
  EMA_REGION_END(load_data);

  // start periodically measuring gpu usage
  std::thread gpu_measure_thread(monitor_gpu_usage, gpu_measure_interval, std::to_string(pid));

  // Initialize the framework
  EMA_REGION_BEGIN(setup);
  framework_setup(framework, keras_model_path, R);
  EMA_REGION_END(setup);

  // Train the model
  EMA_REGION_BEGIN(training);
  framework_train(framework, x, y, x_val, y_val, R);
  EMA_REGION_END(training);
  
  // Finalize and get results
  run_gpu_monitoring = false;
  gpu_measure_thread.join();
  EMA_finalize();

  // Write job info to file
  std::ofstream output_file("output.job_info." + std::to_string(pid));
  if (output_file.is_open()) {
    output_file << "iterations,framework,model_size" << std::endl;
    for (size_t i = 1; i < argc; ++i) {
      output_file << argv[i];
      if (i != argc - 1) {
        output_file << ",";
      } else {
        output_file << std::endl;
      }
    }
    output_file.close();
  } else {
    std::cout << "Unable to write job info to output file" << std::endl;
  }

  return 0;
} 
