#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <nvml.h>
#include "gpu_monitor.h"

void monitor_gpu_usage(int interval, std::string pid) {
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
  std::ofstream output_file("output.GPU_usage." + pid);
  if (output_file.is_open()) {
    for (size_t i = 0; i < gpu_utilization_history.size(); ++i) {
      output_file << gpu_utilization_history[i] << ",";
    }
    output_file.close();
  } else {
    std::cout << "Unable to write GPU usage to output file" << std::endl;
  }
}