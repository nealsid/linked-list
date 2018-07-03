#ifndef PERF_MEASUREMENT_H_
#define PERF_MEASUREMENT_H_

#include <chrono>
#include <fstream>
#include <map>
#include <vector>

#include "mutex_wrapper.h"

using namespace std;
using namespace std::chrono;

namespace perf_measurement {
  
extern map<string, pair<MutexWrapperunique_ptr<vector<nanoseconds>>> measurements;
extern MutexWrapper measurements_lock;

template <typename F, typename... Args>
void measurer(string measurement_name, F toInvoke, Args ...args) {
  auto start = std::chrono::high_resolution_clock::now();
  toInvoke(args...);
  auto end = std::chrono::high_resolution_clock::now();
  nanoseconds time_for_function_call = nanoseconds(end - start);
  if (measurements.find(measurement_name) == measurements.end()) {
    measurements.insert(make_pair(measurement_name, new vector<nanoseconds>(10000)));
  }

  measurements[measurement_name]->push_back(time_for_function_call);
};

void writeMeasurementsToFile(string output_filename);
}

#endif  // PERF_MEASUREMENT_H_
