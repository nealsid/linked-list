#include <optional>
#include <variant>

using namespace std;

template<int N, typename T>
struct LinkedList {
  optional<T> data;
  LinkedList<N - 1, T> remainder;
};

template<typename T>
struct LinkedList<0, T> {
  optional<T> data;
};

template<int N, typename T>
optional<T> find(LinkedList<N, T> l, T data) {
  if (l.data == data) {
    return data;
  }
  return find<N-1>(l.remainder, data);
}

template<int N, typename T>
optional<T> find(LinkedList<1, T> l, T data) {
  if (l.data == data) {
    return data;
  }
  return optional<T>();
}

template<int N, typename T>
int count(LinkedList<N, T> l) {
  return N;
}

template<int N, typename T>
int reallyCount(LinkedList<N, T> l) {
  return 1 + reallyCount(l.remainder);
}

template<typename T>
int reallyCount(LinkedList<1, T> l) {
  return 1;
}

template<int N, typename T>
variant<LinkedList<N, T>, LinkedList<N + 1, T>> insert(LinkedList<N, T> l, int position, T data) {

  if (position == 1) {

  }
  l.remainder = insert(l.remainder, position - 1, data);


}
