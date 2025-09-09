// Minimal Cohen-Sutherland line clipping utility
#pragma once
#include "tabula/Rectangle.h"

namespace tabula {

// Clips the line (x1,y1)-(x2,y2) to the rectangle 'rect'.
// Returns true if the resulting clipped segment is non-empty and updates
// x1,y1,x2,y2 in place.
bool cohenSutherlandClip(const Rectangle &rect, float &x1, float &y1, float &x2,
                         float &y2);

} // namespace tabula
