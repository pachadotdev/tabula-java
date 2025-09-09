#include "tabula/Ruling.h"
#include "tabula/Utils.h"
#include <cassert>
#include <iostream>

using namespace tabula;

int main() {
  // Utils::within
  assert(Utils::within(10.0, 10.05, 0.1));
  assert(!Utils::within(10.0, 11.0, 0.1));

  // Utils::feq
  assert(Utils::feq(1.0, 1.005));
  assert(!Utils::feq(1.0, 1.1));

  // Utils::range
  auto r = Utils::range(3, 7);
  assert(r.size() == 4 && r[0] == 3 && r[3] == 6);

  // Ruling normalize & orientation
  Ruling h(0, 0, 10, 0); // horizontal
  assert(h.horizontal());
  Ruling v(0, 0, 0, 10);
  assert(v.vertical());

  // expand and intersection
  Ruling a(0, 0, 0, 5), b(0, 2, 5, 2); // vertical and horizontal cross at (0,2)
  auto p = a.intersectionPoint(b);
  assert(!std::isnan(p.first) && !std::isnan(p.second));
  assert((int)p.first == 0 && (int)p.second == 2);

  std::cout << "All unit tests passed\n";
  return 0;
}
