#include "static_template_linked_list.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
  LinkedList<5, int> l;

  l.remainder.remainder.data = 5;
  auto srch = find(l, 5);
  cout << ((srch && srch.value() == 5) ? "true" : "false") << endl;
  srch = find(l, 29);
  cout << ((srch && srch.value() == 29) ? "true" : "false") << endl;

  cout << "reallyCount: " << reallyCount(l) << endl;
  cout << "count: " << count(l) << endl;
}
