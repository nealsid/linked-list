#include <iostream>
#include <thread>

#include "linked_list.h"
#include "perf_measurement.h"

using namespace linkedlist;
using namespace std;

void f(int a) {
  cout << a << endl;
}

void noarg() {
  cout << "noarg" << endl;
}

int main(int argc, char* argv[]) {
  //  measurer(f, 5);
  
  LockedLinkedList<int> linkedList;
  vector<thread*> threads;
  
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

  linkedList.DumpListToFile("output.txt");
  perf_measurement::writeMeasurementsToFile("perf_measurements.txt");
  
  // for (int i = 0; i < 400; ++i) {
  //   cout << "PopFronting " << i << endl;
  //   int val = linkedList.PopFront();
  //   cout << "\t" << val << endl;
  // }

  // threads.clear();
  // for (int i = 0; i < 20; ++i) {
  //   threads.push_back(new thread([&,i] () {
  //                                  for (int j = i * 20; j < (i * 20) + 20; j++) {
  //                                    linkedList.PushFront(j);
  //                                  }
  //                                }));
  // }
  // for (auto t : threads) {
  //   t->join();
  // }

  // linkedList.DumpList();
}
