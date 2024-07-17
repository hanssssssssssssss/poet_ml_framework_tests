#include <RInside.h>
#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include "gpu_monitor.h"
#include "framework_test_functions.h"

using Field = std::vector<std::vector<double>>;

std::atomic<float> gpu_utilization(0.0f);
std::atomic<bool> run_gpu_monitoring(true);

int main(int argc, char *argv[]) {
  const int gpu_measure_interval = 1000;
  std::string keras_model = "../src/barite_50ai_all.keras";

  if (argc != 3) {
    std::cout << "Wrong number of arguments!" << std::endl;
    return 1;
  }

  RInside R(argc, argv);

  // Load R util functions
  R.parseEval("source(\"../src/Rfiles/Rfunctions.R\")");

  // Load RDS files
  int iterations = std::stoi(argv[1]);
  Field R_chem_data;
  Field R_tug_data;
  for (int i = 0; i < iterations; i++) {
    R.parseEval("R_temp_data <- import_data(\"../RData/\"," + std::to_string(i + 1) + ")");
    Field temp_field_C = R.parseEval("get_C(R_temp_data)");
    Field temp_field_T = R.parseEval("get_T(R_temp_data)");
    if (i == 0) {
      // Initialize field
      R_chem_data.insert(R_chem_data.end(), 
                         temp_field_C.begin(),
                         temp_field_C.end());
      R_tug_data.insert(R_tug_data.end(),
                        temp_field_T.begin(),
                        temp_field_T.end());
    } else {
      // Insert column wise
      for (int i = 0; i < R_chem_data.size(); i++) {
        R_chem_data[i].insert(R_chem_data[i].begin(),
                              temp_field_C[i].begin(),
                              temp_field_C[i].end());
        R_tug_data[i].insert(R_tug_data[i].begin(),
                             temp_field_T[i].begin(),
                             temp_field_T[i].end());
      }
    }
  }

  std::string framework = argv[2];
  std::chrono::duration<double> time_training(0);
  
  std::thread gpu_measure_thread(monitor_gpu_usage, gpu_measure_interval);

  // Test for keras 3 wit XLA
  if (!framework.compare("keras3")) {
    auto start = std::chrono::high_resolution_clock::now();
    
    R_keras_setup(keras_model, R);
    R_keras_train(R_tug_data, R_chem_data, R);
    
    auto end = std::chrono::high_resolution_clock::now();
    time_training = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  } else if (!framework.compare("test")) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  } else {
    std::cout << "Invalid Framework argument" << std::endl;
  }
  run_gpu_monitoring = false;
  gpu_measure_thread.join();
  std::cout << "Training Time:" << time_training.count() << std::endl;
  return 0;
} 
