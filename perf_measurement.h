#ifndef PERF_MEASUREMENT_H_
#define PERF_MEASUREMENT_H_

#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <vector>

#include "mutex_wrapper.h"

using namespace linkedlist;
using namespace std;
using namespace std::chrono;

namespace perf_measurement {
  
extern map<string, pair<unique_ptr<mutex>,
                        unique_ptr<vector<nanoseconds>>>> measurements;
extern mutex measurements_lock;

template <typename S, typename T>
typename map<S, T>::iterator findOrCreateMapEntry(map<S, T>& map,
                                                  S key,
                                                  mutex& map_mutex);

template <typename F, typename... Args>
void measurer(string measurement_name, F toInvoke, Args ...args) {
  auto start = std::chrono::high_resolution_clock::now();
  toInvoke(args...);
  auto end = std::chrono::high_resolution_clock::now();
  nanoseconds time_for_function_call = nanoseconds(end - start);

  auto measurements_entry = findOrCreateMapEntry(measurements,
                                                 measurement_name,
                                                 measurements_lock);
  lock_guard<mutex> guard(*measurements_entry->second.first);
  measurements_entry->second.second->push_back(time_for_function_call);
};

template <typename S, typename T>
typename std::map<S, T>::iterator findOrCreateMapEntry(map<S, T>& map,
                                                       S key,
                                                       mutex& map_mutex) {
  auto entry = map.find(key);
  if (entry == map.end()) {
    map_mutex.lock();
    entry = map.find(key);
    if (entry == map.end()) {
      auto insertion_result =
          map.insert(make_pair(key,
                               make_pair(new mutex(),
                                         new vector<nanoseconds>(10000))));
      entry = insertion_result.first;
    }
    map_mutex.unlock();
  }
  return entry;
}
void writeMeasurementsToFile(string output_filename);
}

#endif  // PERF_MEASUREMENT_H_
