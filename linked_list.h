#include <memory>
#include <mutex>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include "mutex_wrapper.h"

using namespace std;

namespace linkedlist {
  
template <typename T>
struct LinkedListNode {
  T data;
  LinkedListNode<T>* next;
  LinkedListNode<T>* prev;
};

  
template <typename T>
class LinkedList {
public:
  virtual void PushBack(const T& data) = 0;
  virtual void PushFront(const T& data) = 0;
  
  virtual T PopFront() = 0;
  virtual T PopBack() = 0;
};


template<typename T>
class LockedLinkedList : public LinkedList<T> {
 public:
  LockedLinkedList();
  void PushFront(const T& data);
  void PushBack(const T& data);
  
  T PopFront();
  T PopBack();

  void DumpListToFile(string filename) const;
    
private:
  LinkedListNode<T>* head_;
  MutexWrapper head_dummy_node_mutex_;
  LinkedListNode<T>* tail_;
  MutexWrapper tail_dummy_node_mutex_;
};

#include "linked_list.tcc"
}
