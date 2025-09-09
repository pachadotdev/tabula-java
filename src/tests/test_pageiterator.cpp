#include "tabula/ObjectExtractor.h"
#include "tabula/PageIterator.h"
#include <iostream>

int main() {
  using namespace tabula;
  ObjectExtractor oe;
  std::vector<int> pages = {1, 2, 3};
  PageIterator it(&oe, pages);
  int count = 0;
  while (it.hasNext()) {
    Page p = it.next();
    ++count;
  }
  if (count != 3)
    return 2;
  std::cout << "pageiterator OK\n";
  return 0;
}
