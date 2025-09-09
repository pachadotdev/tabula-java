#pragma once
#include "tabula/Page.h"
#include "tabula/Rectangle.h"
#include <vector>

namespace tabula {
namespace detectors {

class SpreadsheetDetectionAlgorithm {
public:
  std::vector<Rectangle> detect(const Page &page) const;
};

} // namespace detectors
} // namespace tabula
