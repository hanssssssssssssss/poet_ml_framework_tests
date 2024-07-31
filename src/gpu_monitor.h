#ifndef GPU_MONITOR_H
#define GPU_MONITOR_H

#include <atomic>

extern std::atomic<float> gpu_utilization;
extern std::atomic<bool> run_gpu_monitoring;

void monitor_gpu_usage(int interval, std::string pid);

#endif // GPU_MONITOR_H