#include <RInside.h>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <fstream>
#include <atomic>
#include "config.h"
#include "framework_test_functions.h"
#if USE_GPU
  #include "gpu_monitor.h"
#endif
extern "C" {
  #include <EMA.h>
}

// TODO: Use validation data after training
// TODO: Generate Training Data from POET
// TODO: Implement large model
// TODO: Implement tflite
// TODO: Implement tflite quantization

using Field = std::vector<std::vector<double>>;

std::atomic<float> gpu_utilization(0.0f);
std::atomic<bool> run_gpu_monitoring(true);

int main(int argc, char *argv[]) {
  if (argc != 5) {
    std::cout << "Wrong number of arguments!" << std::endl;
    std::cout << "Arguments must be: framework task model_size batch_size" << std::endl;
    return 1;
  }

  // Constants
  const int gpu_measure_interval = 1000;
  const float validation_data_percentage = .05;
  const std::string rfile_path = RFILES_DIR;
  const std::string rdata_path = RDATA_DIR;


  const std::string framework = argv[1];
  const std::string task = argv[2];
  const std::string model = argv[3];
  const int batch_size = std::stoi(argv[4]);

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
  EMA_REGION_DECLARE(process_data);
  //Filter *nvml_filter = EMA_filter_exclude_plugin("NVML");
  //EMA_REGION_DEFINE_WITH_FILTER(&load_data, "load_data", nvml_filter);
  //EMA_REGION_DEFINE_WITH_FILTER(&setup, "setup", nvml_filter);
  //EMA_REGION_DEFINE_WITH_FILTER(&training, "training", nvml_filter);

  EMA_REGION_DEFINE(&load_data, "load_data");
  EMA_REGION_DEFINE(&setup, "setup");
  EMA_REGION_DEFINE(&process_data, "process_data");

  RInside R(argc, argv);

  // Load R util functions
  R.parseEval("source(\"" + rfile_path + "/Rfunctions.R\")");
  R.parseEval("source(\"" + rfile_path + "/Rfunctions_POET.R\")");

  // Load POET data
  EMA_REGION_BEGIN(load_data);
  R.parseEval("require(qs)");
  R.parseEval("qs_data <- qs::qread(\"" + rdata_path + "/Barite_50_Data.qs\")");
  R.parseEval("temp_field_T <- preprocess(qs_data$design)");
  
  if (!task.compare("train")) {
    // split into training and validation data
    R.parseEval("temp_field_C <- preprocess(qs_data$result)");
    R["validation_data_percentage"] = validation_data_percentage;
    //R.parseEval("validation_indices <- get_random_indices(nrow(temp_field_C), validation_data_percentage)");
    R.parseEval("validation_indices <- get_indices_from_file(\"" + rdata_path + "/indices.rds\")");
    R.parseEval("x_train <- temp_field_T[-validation_indices,]");
    R.parseEval("y_train <- temp_field_C[-validation_indices,]");
    R.parseEval("x_val <- temp_field_T[validation_indices,]");
    R.parseEval("y_val <- temp_field_C[validation_indices,]");
  } else if (!task.compare("predict")) {
    // use all available data to predict
    R.parseEval("x_train <- temp_field_T");
  } else {
    std::cout << "Arguments must be: framework task model_size batch_size" << std::endl;
    std::cout << task << " is an invalid task. Choose between [train/predict]" << std::endl;
    return -1;
  }
  Field x = R["x_train"];
  Field y = R["y_train"];
  Field x_val = R["x_val"];
  Field y_val = R["y_val"];
  EMA_REGION_END(load_data);

  // start periodically measuring gpu usage
  #if USE_GPU
    std::thread gpu_measure_thread(monitor_gpu_usage, gpu_measure_interval, std::to_string(pid));
  #endif
  // Initialize the framework
  EMA_REGION_BEGIN(setup);
  framework_setup(framework, model, R);
  EMA_REGION_END(setup);

  // Train the model
  EMA_REGION_BEGIN(process_data);
  if (!task.compare("train")) {
    framework_train(framework, x, y, x_val, y_val, batch_size, R, std::to_string(pid));
  } else if (!task.compare("predict")) {
    framework_predict(framework, x, batch_size, R);
  }
  EMA_REGION_END(process_data);
    
  // Finalize and get results
  run_gpu_monitoring = false;
  #if USE_GPU
    gpu_measure_thread.join();
  #endif
  EMA_finalize();

  // Write job info to file
  std::ofstream output_file("output.job_info." + std::to_string(pid));
  if (output_file.is_open()) {
    output_file << "framework,model,batch_size" << std::endl;
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
