#ifndef LINKEDLIST_MUTEX_WRAPPER_H_
#define LINKEDLIST_MUTEX_WRAPPER_H_

#include <mutex>
#include <string>
#include <thread>

using namespace std;

namespace linkedlist {
class MutexWrapper {
 public:
    MutexWrapper(const string& name);

    MutexWrapper(const MutexWrapper& other) = delete;
    MutexWrapper(MutexWrapper&& other) = delete;
    void operator=(const MutexWrapper& other) = delete;

    void lock();
    bool try_lock();
    void unlock();
    bool locked();
    const string& get_name();
 private:
    mutex m;
    string name;
    thread::id owning_thread_id;
  };

}

#endif // LINKEDLIST_MUTEX_WRAPPER_H_
