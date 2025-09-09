#include "tabula/CohenSutherlandClipping.h"
#include <cmath>

namespace tabula {

// region codes
static const int INSIDE = 0; // 0000
static const int LEFT = 1;   // 0001
static const int RIGHT = 2;  // 0010
static const int BOTTOM = 4; // 0100
static const int TOP = 8;    // 1000

static int computeOutCode(const Rectangle &rect, float x, float y) {
  int code = INSIDE;
  if (x < rect.left)
    code |= LEFT;
  else if (x > rect.left + rect.width)
    code |= RIGHT;
  if (y < rect.top)
    code |= TOP; // Note: y grows downward in PDF coordinates; keep conventional
                 // names
  else if (y > rect.top + rect.height)
    code |= BOTTOM;
  return code;
}

bool cohenSutherlandClip(const Rectangle &rect, float &x1, float &y1, float &x2,
                         float &y2) {
  int out1 = computeOutCode(rect, x1, y1);
  int out2 = computeOutCode(rect, x2, y2);
  bool accept = false;

  while (true) {
    if (!(out1 | out2)) { // both inside
      accept = true;
      break;
    } else if (out1 & out2) { // logical AND != 0 => trivially reject
      break;
    } else {
      // pick one outside
      int outcodeOut = out1 ? out1 : out2;
      float x, y;

      if (outcodeOut & TOP) {
        // intersect with top
        x = x1 + (x2 - x1) * (rect.top - y1) / (y2 - y1);
        y = rect.top;
      } else if (outcodeOut & BOTTOM) {
        x = x1 + (x2 - x1) * (rect.top + rect.height - y1) / (y2 - y1);
        y = rect.top + rect.height;
      } else if (outcodeOut & RIGHT) {
        y = y1 + (y2 - y1) * (rect.left + rect.width - x1) / (x2 - x1);
        x = rect.left + rect.width;
      } else { // LEFT
        y = y1 + (y2 - y1) * (rect.left - x1) / (x2 - x1);
        x = rect.left;
      }

      if (outcodeOut == out1) {
        x1 = x;
        y1 = y;
        out1 = computeOutCode(rect, x1, y1);
      } else {
        x2 = x;
        y2 = y;
        out2 = computeOutCode(rect, x2, y2);
      }
    }
  }

  return accept;
}

} // namespace tabula
