#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include <atomic>

extern std::atomic<float> gpu_utilization;
extern std::atomic<bool> run_gpu_monitoring;

void monitor_gpu_usage(int interval);

#endif // GLOBAL_VARS_H