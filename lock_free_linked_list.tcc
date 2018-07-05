template <typename T>
LockFreeLinkedList<T>::LockFreeLinkedList() :
  head_(new LockFreeLinkedListDummyNode<T>()),
  tail_(new LockFreeLinkedListDummyNode<T>()) {

  head_->next.store(tail_);
  head_->prev.store(nullptr);
  tail_->prev.store(head_);
  tail_->next.store(nullptr);
}

template <typename T>
void LockFreeLinkedList<T>::PushFront(const T& data) {
  //  cout << "PushFront " << data << endl;
  LinkedListNode<T>* node = new LinkedListNode<T>();
  node->data = data;
  node->prev = head_;

  LinkedListNode<T>* nodeNext;
  do {
    nodeNext = head_->next.load();
    node->next = nodeNext;
  } while (!head_->next.compare_exchange_weak(nodeNext, node));

  // Having the next statement outside the loop can make the list
  // slightly inconsistent.  Someone traversing prev pointers before
  // this line is executed will not see the new node added to the
  // list.  It can't be in the loop because if the CAS fails, we'll
  // have updated a node's prev pointer that shouldn't point to the
  // new node.  Pictorially, it can look like this:

  // Linked list before any nodes are inserted (just the dummy head &
  // tail nodes):

  // nullptr <--  |--|-|    -->    |--|-|    -->     nullptr
  //              |  | |           |  | |        
  //              |--|-|    <--    |--|-|


  // After the CAS loop above but before the next statement below:

  // nullptr <--  |--|-|    -->    |--|-|    -->    |--|-|    -->  nullptr
  //              |  | |           |  | |           |  | |                
  //              |--|-|    <--    |--|-|           |--|-|            
  //                   ^                             |
  //                    \___________________________/


  // Suppose two threads complete the CAS loop but neither has hit the
  // next statement below. This is what the list looks like:
  
  // nullptr <--  |--|-|    -->    |--|-|    -->    |--|-|    -->   |--|-|    -->  nullptr
  //              |  | |           |  | |           |  | |          |  | |                
  //              |--|-|    <--    |--|-|           |--|-|          |--|-|            
  //                 ^ ^                             |                ^
  //                 |  \___________________________/                 |
  //                 |                                                |
  //                  \______________________________________________/
			
  // Since each thread is pointing to the node it just inserted, and
  // the next pointers are correct after the CAS loop, each will fix
  // the prev pointer of the node after the one it just inserted, so
  // it's "correct" with no race conditions (quotes because of the
  // inconsistency in what a thread sees depending on whether it's
  // traversing forward or backward)
  node->next->prev = node;
}

template <typename T>
void LockFreeLinkedList<T>::PushBack(const T& data) {
  LinkedListNode<T>* node = new LinkedListNode<T>();
  node->data = data;
  node->next = tail_;

  LinkedListNode<T>* nodePrev;
  do {
    nodePrev = tail_->prev.load();
    node->prev = nodePrev;
  } while (!tail_->prev.compare_exchange_weak(nodePrev, node));
  // See above about inconsistency due to CAS loop.
  node->prev->next = node;
}

template <typename T>
bool LockFreeLinkedList<T>::IsEmpty() {
  return head_->next == tail_;
}

template <typename T>
T LockFreeLinkedList<T>::PopFront() {
  LinkedListNode<T>* node;
  T retVal;
  
  do {
    node = head_->next.load();
    retVal = node->data;
  } while (!head_->next.compare_exchange_weak(node, node->next));


  // TODO: There is a race condition if this function fully executes
  // while PushFront is between the CAS loop and the final statement.
  //
  // Suppose you start with this linked list:
  //
  // nullptr <--  |--|-|    -->    |--|-|    -->     nullptr
  //              |  | |           |  | |        
  //              |--|-|    <--    |--|-|
  //
  // Now the CAS loop of PushFront executes but not the final pointer update:
  //
  // nullptr <--  |--|-|    -->    |--|-|    -->    |--|-|    -->  nullptr
  //              |  | |           |  | |           |  | |                
  //              |--|-|    <--    |--|-|           |--|-|            
  //                   ^                             |
  //                    \___________________________/

  // Now PopFront executes the CAS loop:
  //
  //                     ___________________________
  //                    /                           \
  //                   /                            \/
  // nullptr <--  |--|-|           |--|-|    -->    |--|-|    -->  nullptr
  //              |  | |           |  | |           |  | |                
  //              |--|-|    <--    |--|-|           |--|-|            
  //                   ^                             |
  //                    \___________________________/
  //
  // A few cases:
  //
  // 1) PushFront finishes completely before we execute, in which case
  // the list is ok, because the prev node of node->next will end up
  // setting to head_ after this function.
  //
  // 2) If the rest of PopFront executes before PushFront finishes,
  // when PushFront finishes, there will be a dereference of deleted
  // memory.
  // 
  // 3) If the first line of the rest of PopFront executes, then
  // PushFront finishes, we'll be bad, because we'll have overwritten
  // the prev pointer of node->next to point to a deleted node.
  //
  // Still scratching my head over this. 
  node->next->prev = head_;
  delete node;
  return std::move(retVal);
}

template <typename T>
T LockFreeLinkedList<T>::PopBack() {
  LinkedListNode<T>* node;
  T retVal;
  
  do {
    node = tail_->prev.load();
    retVal = node->data;
  } while (!tail_->prev.compare_exchange_weak(node, node->prev));
  
  node->prev->next = tail_;
  delete node;
  return std::move(retVal);
}

template <typename T>
void LockFreeLinkedList<T>::DumpListToFile(string filename) const {
  ofstream of(filename);
  LinkedListNode<T>* node = head_->next;
  while (node != tail_) {
    of << node->data << endl;
    node = node->next;
  }
  of.close();
  cout << endl;
}

