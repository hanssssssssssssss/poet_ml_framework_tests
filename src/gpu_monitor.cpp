#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <nvml.h>
#include "gpu_monitor.h"

void monitor_gpu_usage(int interval, std::string output_folder) {
  nvmlReturn_t result;
  nvmlDevice_t device;
  nvmlUtilization_t utilization;
  std::vector<int> gpu_utilization_history;

  result = nvmlInit();
  if (result != NVML_SUCCESS) {
    std::cerr << "Failed to initialize NVML: " << nvmlErrorString(result) << std::endl;
  }
  

  result = nvmlDeviceGetHandleByIndex(0, &device);
  if (result != NVML_SUCCESS) {
    std::cerr << "Failed to get device handle: " << nvmlErrorString(result) << std::endl;
    nvmlShutdown();
  }

  while (run_gpu_monitoring) {
    result = nvmlDeviceGetUtilizationRates(device, &utilization);
    if (result == NVML_SUCCESS) {
      gpu_utilization.store(utilization.gpu);
      gpu_utilization_history.push_back(utilization.gpu);
    } else {
      std::cerr << "Failed to get GPU utilization: " << nvmlErrorString(result) << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
  }

  // Write the collected GPU utilization history to csv
  std::ofstream output_file(output_folder + "/gpu_usage.csv");
  if (output_file.is_open()) {
    for (size_t i = 0; i < gpu_utilization_history.size(); ++i) {
      output_file << gpu_utilization_history[i] << ",";
    }
    output_file.close();
    std::cout << "GPU measurements written to " << output_folder << std::endl;
  } else {
    std::cout << "Unable to open " << output_folder << std::endl;
  }
}