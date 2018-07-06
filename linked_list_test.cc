#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "linked_list.h"
#include "perf_measurement.h"

using namespace linkedlist;
using namespace std;

class LinkedListTest : public ::testing::Test {
  virtual void SetUp() {
    perf_measurement::clearResults();
  }
protected:
  int NUM_ELEMENTS = 1000000;
};

TEST_F(LinkedListTest, SingleThreadedTest) {
  LockedLinkedList<int> linkedList;

  for (int i = NUM_ELEMENTS - 1; i >= 0; --i) {
    linkedList.PushFront(i);
  }

  vector<int> ints;
  ints.reserve(NUM_ELEMENTS);

  for (int i = 0; i < NUM_ELEMENTS; ++i) {
    ints.push_back(linkedList.PopFront());
  }

  ASSERT_TRUE(linkedList.IsEmpty());

  for (int i = 0; i < NUM_ELEMENTS; ++i) {
    ASSERT_EQ(ints[i], i);
  }
  perf_measurement::writeMeasurementsToFile("no_contention_measurements.txt");
}

TEST_F(LinkedListTest, MultithreadedTest) {
  LockedLinkedList<int> linkedList;
  vector<thread*> threads;
  
  int NUM_THREADS = 10;
  int NUM_ELEMENTS_PER_THREAD = NUM_ELEMENTS / NUM_THREADS;
  // 10 threads, 100000 distinct numbers each.
  std::condition_variable thread_start;
  std::mutex thread_start_mutex;

  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.push_back(new thread([&,i] () {
				   {
				     unique_lock<mutex> lock(thread_start_mutex);
				     thread_start.wait(lock);
				   }
				   int thread_starting_point = i * NUM_ELEMENTS_PER_THREAD;
				   for (int j = thread_starting_point;
					j < thread_starting_point + NUM_ELEMENTS_PER_THREAD;
					j++) {
				     linkedList.PushFront(j);
				   }
				 }));
  }
			     
  // I am seeing that the main thread grabs thread_start_mutex before
  // the last thread created above, preventing that thread from
  // waiting on the condition variable in time.
  std::this_thread::sleep_for((milliseconds)500);
  perf_measurement::measurer("insertion-1m", [&] () {
					       thread_start_mutex.lock();
					       thread_start.notify_all();
					       thread_start_mutex.unlock();
					       for (auto t : threads) {
						 t->join();
					       }
					     });
  // Now grab all the numbers from the list, sort and compare to what
  // we expect.
  // Initializing the vector to 1M elements and then immediately
  // overwriting it is wasteful, but it lets me be really clever with
  // std::generate to fill the vector with elements from the list.
  vector<int> ints(NUM_ELEMENTS);
  std::generate(ints.begin(), ints.end(),
                [&] () {
                  return linkedList.PopFront();
                });

  ASSERT_TRUE(linkedList.IsEmpty());
  
  std::sort(ints.begin(), ints.end());

  for (int i = 0; i < NUM_ELEMENTS; ++i) {
    ASSERT_EQ(ints[i], i);
  }
  perf_measurement::writeMeasurementsToFile("contention_measurements.txt");
}

TEST_F(LinkedListTest, SingleThreadedLockFreeTest) {
  LockFreeLinkedList<int> linkedList;

  for (int i = NUM_ELEMENTS - 1; i >= 0; --i) {
    linkedList.PushFront(i);
  }

  vector<int> ints;
  ints.reserve(NUM_ELEMENTS);

  for (int i = 0; i < NUM_ELEMENTS; ++i) {
    ints.push_back(linkedList.PopFront());
  }

  ASSERT_TRUE(linkedList.IsEmpty());

  for (int i = 0; i < NUM_ELEMENTS; ++i) {
    ASSERT_EQ(ints[i], i);
  }
  perf_measurement::writeMeasurementsToFile("lock_free_single_thread_contention_measurements.txt");
}

TEST_F(LinkedListTest, MultithreadedLockFreeTest) {
  LockFreeLinkedList<int> linkedList;
  vector<thread*> threads;
  
  int NUM_THREADS = 10;
  int NUM_ELEMENTS_PER_THREAD = NUM_ELEMENTS / NUM_THREADS;
  // 10 threads, 100000 distinct numbers each.
  std::condition_variable thread_start;
  std::mutex thread_start_mutex;
  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.push_back(new thread([&,i] () {
				   {
				     unique_lock<mutex> lock(thread_start_mutex);
				     thread_start.wait(lock);
				   }
				   int thread_starting_point = i * NUM_ELEMENTS_PER_THREAD;
				   for (int j = thread_starting_point;
					j < thread_starting_point + NUM_ELEMENTS_PER_THREAD;
					j++) {
				     linkedList.PushFront(j);
				   }
				 }));
  }
			     

  // See above for comment about sleep.
  std::this_thread::sleep_for((milliseconds)500);
  perf_measurement::measurer("insertion-1m", [&] () {
					       thread_start_mutex.lock();
					       thread_start.notify_all();
					       thread_start_mutex.unlock();
					       for (auto t : threads) {
						 t->join();
					       }
					     });

  // Now grab all the numbers from the list, sort and compare to what
  // we expect.
  // Initializing the vector to 1M elements and then immediately
  // overwriting it is wasteful, but it lets me be really clever with
  // std::generate to fill the vector with elements from the list.
  vector<int> ints(NUM_ELEMENTS);
  std::generate(ints.begin(), ints.end(),
                [&] () {
                  return linkedList.PopFront();
                });

  ASSERT_TRUE(linkedList.IsEmpty());
  
  std::sort(ints.begin(), ints.end());

  for (int i = 0; i < NUM_ELEMENTS; ++i) {
    ASSERT_EQ(ints[i], i);
  }
  perf_measurement::writeMeasurementsToFile("lockfree_multithreaded_contention_measurements.txt");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

 
