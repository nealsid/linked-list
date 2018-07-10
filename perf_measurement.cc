#include <mutex>

#include "perf_measurement.h"

using namespace std;

namespace perf_measurement {

map<string, pair<unique_ptr<mutex>,
                 unique_ptr<vector<nanoseconds>>>> measurements;
mutex measurements_lock;

void clearResults() {
  lock_guard<mutex> guard(measurements_lock);
  measurements.clear();
}

void writeMeasurementsToFile(string output_filename) {
  ofstream of(output_filename, ios::out | ios::app);
  for (const auto& measurement : measurements) {
    of << measurement.first;
    nanoseconds sum;
    for_each (measurement.second.second->begin(),
              measurement.second.second->end(),
              [&] (nanoseconds x) {
                sum += x;
              });
    auto number_of_calls = measurement.second.second->size();
    of << "\t" << number_of_calls
       << "\t" << sum.count()
       << "\t" << sum.count() / number_of_calls << endl;
  }
}


}
