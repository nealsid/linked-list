#include <iostream>
#include <thread>

#include "mutex_wrapper.h"
#include "perf_measurement.h"

using namespace perf_measurement;
using namespace std;

namespace linkedlist {

MutexWrapper::MutexWrapper(const string& name) {
  this->name = name;
}

void MutexWrapper::lock() {
  //  cout << "Locking " << this->name << " by thread " << this_thread::get_id() << endl;
  measurer("lock", std::bind(&mutex::lock, &m));
  this->owning_thread_id = this_thread::get_id();
}

bool MutexWrapper::try_lock() {
  return m.try_lock();
}

void MutexWrapper::unlock() {
  //   cout << "Unlocking " << this->name << endl;
  measurer("unlock", std::bind(&mutex::unlock, &m));
}

const string& MutexWrapper::get_name() {
  return this->name;
}

}

