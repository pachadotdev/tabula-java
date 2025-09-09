#pragma once
#include "tabula/Page.h"
#include "tabula/Table.h"
#include <vector>

namespace tabula {

class BasicExtractionAlgorithm {
public:
  // Extract tables from a page using the detector to form a grid
  std::vector<Table> extract(const Page &page, float minColumnWidth = 1.0f,
                             float minRowHeight = 1.0f) const;
};

} // namespace tabula
