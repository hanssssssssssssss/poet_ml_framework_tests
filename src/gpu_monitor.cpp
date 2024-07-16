#include <chrono>
#include <iostream>
#include <thread>
#include <nvml.h>
#include "gpu_monitor.h"

void monitor_gpu_usage(int interval) {
  nvmlReturn_t result;
  nvmlDevice_t device;
  nvmlUtilization_t utilization;

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
      std::cout << "GPU Utilization: " << utilization.gpu << "%" << std::endl;
    } else {
      std::cerr << "Failed to get GPU utilization: " << nvmlErrorString(result) << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
  }
}