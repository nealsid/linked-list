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
};

TEST_F(LinkedListTest, SingleThreadedTest) {
  LockedLinkedList<int> linkedList;
  for (int i = 1000000; i >= 0; --i) {
    linkedList.PushFront(i);
  }
  vector<int> ints;
  ints.reserve(1000000);
  for (int i = 0; i < 1000000; ++i) {
    ints.push_back(linkedList.PopFront());
  }
  for (int i = 0; i < 1000000; ++i) {
    ASSERT_EQ(ints[i], i);
  }
  perf_measurement::writeMeasurementsToFile("no_contention_measurements.txt");
}

TEST_F(LinkedListTest, MultithreadedTest) {
  LockedLinkedList<int> linkedList;
  vector<thread*> threads;
  
  // 10 threads, 100000 distinct numbers each.
  for (int i = 0; i < 10; ++i) {
    threads.push_back(new thread([&,i] () {
                                   for (int j = i * 100000; j < (i * 100000) + 100000; j++) {
                                       linkedList.PushFront(j);
                                   }
                                 }));
  }
  
  for (auto t : threads) {
    t->join();
  }

  // Now grab all the numbers from the list, sort and compare to what
  // we expect.
  // Initializing the vector to 1M elements and then immediately
  // overwriting it is wasteful, but it lets me be really clever with
  // std::generate to fill the vector with elements from the list.
  vector<int> ints(1000000);
  std::generate(ints.begin(), ints.end(),
                [&] () {
                  return linkedList.PopFront();
                });

  std::sort(ints.begin(), ints.end());

  for (int i = 0; i < 1000000; ++i) {
    ASSERT_EQ(ints[i], i);
  }
  perf_measurement::writeMeasurementsToFile("contention_measurements.txt");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

 
