#pragma once
#include "tabula/Ruling.h"
#include <vector>

namespace tabula {

// Simplified engine: instead of parsing PDF graphics, accept line segments
// and store rulings that exceed a minimum length.
class ObjectExtractorStreamEngine {
public:
  explicit ObjectExtractorStreamEngine(float minLength = 0.01f)
      : minLength_(minLength) {}

  // Add a line segment; the engine will convert it to a Ruling and store it
  void addSegment(float x1, float y1, float x2, float y2) {
    Ruling r(x1, y1, x2, y2);
    if (r.length() > minLength_)
      rulings_.push_back(r);
  }

  const std::vector<Ruling> &getRulings() const { return rulings_; }

  void clear() { rulings_.clear(); }

private:
  std::vector<Ruling> rulings_;
  float minLength_;
};

} // namespace tabula
