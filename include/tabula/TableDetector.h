#pragma once
#include "tabula/Page.h"
#include "tabula/Rectangle.h"
#include <vector>

namespace tabula {

class TableDetector {
public:
  // Given a page, returns rectangular cells inferred from separators
  static std::vector<Rectangle> detectCells(const Page &page,
                                            float minColumnWidth = 1.0f,
                                            float minRowHeight = 1.0f);
};

} // namespace tabula
