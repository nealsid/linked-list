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
  
// map_measurements is a map of a key name to a pair of (mutex,
// vector<nanoseconds>) The map is protected by measurements_lock, but
// that's only required to be taken if you are adding an entry to the
// map.  Otherwise, consumers can just look up the entry they're
// looking for and lock the individual mutex associated with each
// vector. If they don't find it,
//    1) lock measurements_lock
//    2) repeat the lookup
//    3) add the mutex/vector to the map if it still isn't there.
//
extern map<string, pair<unique_ptr<mutex>,
                        unique_ptr<vector<nanoseconds>>>> measurements;
extern mutex measurements_lock;

template <typename S, typename T>
typename map<S, T>::iterator findOrCreateMapEntry(map<S, T>& map,
                                                  S key,
                                                  mutex& map_mutex);

template <typename F, typename... Args>
void measurer(string measurement_name, F toInvoke, Args ...args) {

  // Record time before and after function invocation.
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
  // Look for the key - if we find it, return the iterator. Otherwise,
  // lock the map mutex and add it.
  auto entry = map.find(key);
  if (entry == map.end()) {
    lock_guard<mutex> guard(map_mutex);
    entry = map.find(key);
    if (entry == map.end()) {
      auto insertion_result =
          map.insert(make_pair(key,
                               make_pair(new mutex(),
                                         new vector<nanoseconds>(10000))));
      entry = insertion_result.first;
    }
  }
  return entry;
}

void writeMeasurementsToFile(string output_filename);
void clearResults();
}

#endif  // PERF_MEASUREMENT_H_
