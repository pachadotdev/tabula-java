#include "tabula/CohenSutherlandClipping.h"
#include "tabula/Rectangle.h"
#include <cassert>
#include <iostream>

int main() {
  using namespace tabula;
  Rectangle r{0, 0, 100, 100};
  float x1 = -10, y1 = 50, x2 = 150, y2 = 50;
  bool ok = cohenSutherlandClip(r, x1, y1, x2, y2);
  if (!ok)
    return 2;
  // clipped segment should be within [0,100]
  if (x1 < 0.0f - 1e-6f || x2 > 100.0f + 1e-6f)
    return 3;
  std::cout << "clipping OK\n";
  return 0;
}
