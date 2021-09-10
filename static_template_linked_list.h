#include <optional>

using namespace std;

template<int N, typename T>
struct LinkedList {
  optional<T> data;
  LinkedList<N - 1, T> remainder;
};

template<typename T>
struct LinkedList<1, T> {
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
