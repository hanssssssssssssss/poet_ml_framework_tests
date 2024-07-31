#include <RInside.h>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include "gpu_monitor.h"
#include "framework_test_functions.h"
extern "C" {
  #include <EMA.h>
}

using Field = std::vector<std::vector<double>>;

std::atomic<float> gpu_utilization(0.0f);
std::atomic<bool> run_gpu_monitoring(true);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Wrong number of arguments!" << std::endl;
    return 1;
  }

  // Constants
  const int gpu_measure_interval = 1000;
  const std::string keras_model_path = "../src/barite_50ai_all.keras";
  const pid_t pid = getpid();
  
  const int iterations = std::stoi(argv[1]);
  const std::string framework = argv[2];

  std::cout << "Process ID: " << getpid() << std::endl;
  std::cout << "Uses Framework: " << framework << std::endl; 

  // Initialize EMA
  int ema_err = EMA_init(NULL);
  if(ema_err) {
    std::cout << "EMA initalization failed" << std::endl;
    return 1;
  }

  EMA_REGION_DECLARE(setup);
  EMA_REGION_DECLARE(training);
  EMA_REGION_DEFINE(&setup, "setup");
  EMA_REGION_DEFINE(&training, "training");

  RInside R(argc, argv);

  // Load R util functions
  R.parseEval("source(\"../src/Rfiles/Rfunctions.R\")");

  // Load RDS files
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

  std::thread gpu_measure_thread(monitor_gpu_usage, gpu_measure_interval, std::to_string(pid));

  // Initialize the framework
  EMA_REGION_BEGIN(setup);
  framework_setup(framework, keras_model_path, R);
  EMA_REGION_END(setup);

  // Train the model
  EMA_REGION_BEGIN(training);
  framework_train(framework, R_tug_data, R_chem_data, R);
  EMA_REGION_END(training);
  
  // Get results
  run_gpu_monitoring = false;
  gpu_measure_thread.join();
  EMA_finalize();
  return 0;
} 
