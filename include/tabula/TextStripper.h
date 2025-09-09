#pragma once
#include "tabula/TextChunk.h"
#include "tabula/TextElement.h"
#include <vector>

namespace tabula {

// Very small TextStripper: groups TextElement into TextChunk lines by vertical
// proximity (elements whose vertical centers are within `lineThreshold` are
// grouped). Within a line, elements are sorted by left coordinate.
class TextStripper {
public:
  explicit TextStripper(float lineThreshold = 2.0f)
      : lineThreshold_(lineThreshold) {}

  std::vector<TextChunk> strip(const std::vector<TextElement> &elements) const;

private:
  float lineThreshold_;
};

} // namespace tabula
