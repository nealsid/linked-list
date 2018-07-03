
#include "perf_measurement.h"

namespace perf_measurement {

map<string, pair<MutexWrapper, unique_ptr<vector<nanoseconds>>>> measurements;

void writeMeasurementsToFile(string output_filename) {
  ofstream of(output_filename, ios::out | ios::trunc);
  for (const auto& measurement : measurements) {
    of << measurement.first;
    nanoseconds sum;
    for_each (measurement.second->begin(), measurement.second->end(),
              [&] (nanoseconds x) {
                sum += x;
              });
    auto number_of_calls = measurement.second->size();
    of << "\t" << number_of_calls
       << "\t" << sum.count()
       << "\t" << sum.count() / number_of_calls << endl;
  }
}


}
