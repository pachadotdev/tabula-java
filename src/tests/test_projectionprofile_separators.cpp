#include "tabula/ProjectionProfile.h"
#include "tabula/Rectangle.h"
#include <cassert>
#include <iostream>

using namespace tabula;

int main() {
  // create synthetic rectangles (columns) inside area
  Rectangle area(0, 0, 100, 100);
  std::vector<Rectangle> elems;
  // columns at x ranges: 0-20, 30-50, 60-80
  elems.emplace_back(10, 0, 20, 10);
  elems.emplace_back(10, 30, 20, 10);
  elems.emplace_back(10, 60, 20, 10);

  ProjectionProfile p(area, elems, 3.0f, 3.0f);
  auto vSeps = p.findVerticalSeparators();
  assert(!vSeps.empty());
  std::cout << "test_projectionprofile_separators: OK (vSeps=" << vSeps.size()
            << ")\n";
  return 0;
}
