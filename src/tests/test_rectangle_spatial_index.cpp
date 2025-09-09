#include "tabula/RectangleSpatialIndex.h"
#include <cassert>
#include <iostream>

using namespace tabula;

int main() {
  RectangleSpatialIndex<Rectangle> si(10.0f);
  // insert some rectangles
  si.add(Rectangle(0, 0, 5, 5));
  si.add(Rectangle(20, 20, 5, 5));
  si.add(Rectangle(8, 8, 4, 4));

  // query an area that overlaps first and third
  Rectangle q(0, 0, 10, 10);
  auto res = si.query(q);
  assert(res.size() == 2);
  std::cout << "test_rectangle_spatial_index: OK (hits=" << res.size() << ")\n";
  return 0;
}
