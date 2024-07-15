#include <chrono>
#include <thread>
#include "global_vars.h"

int collect_gpu_stats(int interval) {
  while (run_gpu_monitoring) {
    gpu_usage_avg += interval;
    std::cout << gpu_usage_avg << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
  }
  return 0;
}