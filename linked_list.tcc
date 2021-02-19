template <typename T>
LockedLinkedList<T>::LockedLinkedList() :
  head_(make_shared<LinkedListNode<T>>()),
  tail_(make_shared<LinkedListNode<T>>()),
  head_dummy_node_mutex_("head mutex"),
  tail_dummy_node_mutex_("tail mutex") {
  head_->next = tail_;
  head_->prev = nullptr;
  tail_->prev = head_;
  tail_->next = nullptr;
}

template <typename T>
void LockedLinkedList<T>::PushFront(const T& data) {
  auto node = make_shared<LinkedListNode<T>>();
  node->data = data;

  lock_guard<MutexWrapper> lock(head_dummy_node_mutex_);
  node->next = head_->next;
  node->next->prev = node;
  head_->next = node;
  node->prev = head_;
}

template <typename T>
void LockedLinkedList<T>::PushFront(T&& data) {
  this->PushFrontHelper(data);
}

template <typename T>
template <typename DataReference>
void LockedLinkedList<T>::PushFrontHelper(DataReference&& data) {
  auto node = make_shared<LinkedListNode<T>>();
  node->data = data;

  lock_guard<MutexWrapper> lock(head_dummy_node_mutex_);
  node->next = head_->next;
  node->next->prev = node;
  head_->next = node;
  node->prev = head_;
}

template <typename T>
void LockedLinkedList<T>::PushBack(const T& data) {
  auto node = make_shared<LinkedListNode<T>>();
  node->data = data;

  lock_guard<MutexWrapper> lock(tail_dummy_node_mutex_);
  node->prev = tail_->prev;
  node->prev->next = node;
  tail_->prev = node;
  node->next = tail_;
}

template <typename T>
void LockedLinkedList<T>::PushBack(const T&& data) {
  return this->PushBack(data);
}

template <typename T>
bool LockedLinkedList<T>::IsEmpty() {
  return head_->next == tail_;
}

template <typename T>
T LockedLinkedList<T>::PopFront() {
  shared_ptr<LinkedListNode<T>> node;

  {
    lock_guard<MutexWrapper> lock(head_dummy_node_mutex_);
    node = head_->next;
    head_->next = node->next;
    head_->next->prev = head_;
  }

  return node->data;
}

template <typename T>
T LockedLinkedList<T>::PopBack() {
  shared_ptr<LinkedListNode<T>> node;

  {
    lock_guard<MutexWrapper> lock(tail_dummy_node_mutex_);
    node = tail_->prev;
    tail_->prev = node->prev;
    tail_->prev->next = tail_;
  }

  return node->data;
}

template <typename T>
void LockedLinkedList<T>::DumpListToFile(string filename) const {
  ofstream of(filename);
  auto node = head_->next;
  while (node != tail_) {
    of << node->data << endl;
    node = node->next;
  }
  of.close();
  cout << endl;
}
