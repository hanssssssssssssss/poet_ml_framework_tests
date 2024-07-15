#include <RInside.h>
#include <chrono>
#include <string>
#include <thread>
#include "measure_gpu.h"
#include "global_vars.h"

using Field = std::vector<std::vector<double>>;

std::atomic<int> gpu_usage_avg(0);
std::atomic<bool> run_gpu_monitoring(true);

int main(int argc, char *argv[]) {
  const int interval = 100;

  if (argc != 3) {
    std::cout << "Wrong number of arguments!" << std::endl;
    return 1;
  }

  RInside R(argc, argv);

  // Load R util functions
  R.parseEval("source(\"../src/Rfiles/Rfunctions.R\")");

  // Load RDS files
  int iterations = std::stoi(argv[1]);
  std::vector<Field> R_chem_data(iterations);
  std::vector<Field> R_tug_data(iterations);
  for (int i = 0; i < iterations; i++) {
    R.parseEval("R_temp_data <- import_data(\"../RData/\"," + std::to_string(i + 1) + ")");
    Field temp_field_C = R.parseEval("get_C(R_temp_data)");
    Field temp_field_T = R.parseEval("get_T(R_temp_data)");
    R_tug_data[i] = temp_field_C;
    R_chem_data[i] = temp_field_T;
  }
  int n_rows = R_chem_data[0][0].size();

  std::string framework = argv[2];
  std::chrono::duration<double> time_inference(0);
  std::chrono::duration<double> time_training(0);
  
  std::thread gpu_measure_thread(collect_gpu_stats, interval);

  // Test for keras 3 wit XLA
  if (!framework.compare("keras3")) {
    // setup
    R.parseEval("source(\"../src/Rfiles/Rkeras3.R\")");
    R.parseEval("model <- initiate_model_xla(\"../src/barite_50ai_all.keras\")");
    R.parseEval("print(gpu_info())");

    // predict + train iterations
    for (int i = 0; i < iterations; i++) {
      std::cout << "Iteration: " << std::to_string(i) << std::endl;
      auto start = std::chrono::high_resolution_clock::now();
      // Load the data back to R
      R["TMP"] = R_tug_data[i];
      R.parseEval("predictors <- setNames(data.frame(TMP), colnames)");
      
      std::cout << "Predict:" << std::endl;
      R.parseEval("predictions <- prediction_step(model, predictors)");
      auto end = std::chrono::high_resolution_clock::now();
      time_inference += std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

      std::cout << "Train:" << std::endl;
      start = std::chrono::high_resolution_clock::now();
      R["TMP"] = R_chem_data[i];
      R.parseEval("targets <- setNames(data.frame(TMP), colnames)");
      R.parseEval("model <- training_step(model, predictors, targets)");
      end = std::chrono::high_resolution_clock::now();
      time_training += std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    }
  } else if (!framework.compare("test")) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  } else {
    std::cout << "Invalid Framework argument" << std::endl;
  }
  run_gpu_monitoring = false;
  gpu_measure_thread.join();
  std::cout << "Inference Time:" << time_inference.count() << std::endl;
  std::cout << "Training Time:" << time_training.count() << std::endl;
  return 0;
} 
