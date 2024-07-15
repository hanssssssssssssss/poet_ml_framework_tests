#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include <atomic>

extern std::atomic<int> gpu_usage_avg;
extern std::atomic<bool> run_gpu_monitoring;

#endif // GLOBAL_VARS_H