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
  shared_ptr<LinkedListNode<T>> next;
  shared_ptr<LinkedListNode<T>> prev;
};

template <typename T>
class LinkedList {
public:
  virtual void PushBack(const T& data) = 0;
  virtual void PushBack(T&& data) { }
  virtual void PushFront(const T& data) = 0;
  virtual void PushFront(T&& data) { }
  virtual bool IsEmpty() = 0;

  virtual T PopFront() = 0;
  virtual T PopBack() = 0;
};


template<typename T>
class LockedLinkedList : public LinkedList<T> {
public:
  LockedLinkedList();
  void PushFront(const T& data);
  void PushBack(const T& data);
  void PushBack(T&& data);
  void PushFront(T&& data);

  bool IsEmpty();

  T PopFront();
  T PopBack();

void DumpListToFile(string filename) const;

private:
  std::shared_ptr<LinkedListNode<T>> head_;
  MutexWrapper head_dummy_node_mutex_;
  std::shared_ptr<LinkedListNode<T>> tail_;
  MutexWrapper tail_dummy_node_mutex_;
};

// Dummy nodes for the lock free list. It inherits from LinkedListNode
// because the head dummy node can point to a linked list node OR a
// the dummy tail node when the list is empty.
template <typename T>
struct LockFreeLinkedListDummyNode : LinkedListNode<T> {
  atomic<LinkedListNode<T>*> next;
  atomic<LinkedListNode<T>*> prev;
};

template<typename T>
class LockFreeLinkedList : public LinkedList<T> {
public:
  LockFreeLinkedList();
  void PushFront(const T& data);
  void PushBack(const T& data);
  void PushFront(T&& data);
  void PushBack(T&& data);
  bool IsEmpty();

  T PopFront();
  T PopBack();

  void DumpListToFile(string filename) const;

private:
  LockFreeLinkedListDummyNode<T>* head_;
  LockFreeLinkedListDummyNode<T>* tail_;

  // Helper method to use universal references so we don't have to
  // duplicate code.
  template <typename DataReference>
  void PushFrontHelper(DataReference&& data);
};

#include "linked_list.tcc"
//#include "lock_free_linked_list.tcc"
}
