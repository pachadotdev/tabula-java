#pragma once
#include "tabula/Cell.h"
#include "tabula/Page.h"
#include "tabula/Ruling.h"
#include "tabula/Table.h"
#include <vector>

namespace tabula {

class SpreadsheetExtractionAlgorithm {
public:
  std::vector<Table> extract(const Page &page) const;
  // additional helper APIs (kept minimal for port)
  static std::vector<Cell> findCells(const std::vector<Ruling> &horizontal,
                                     const std::vector<Ruling> &vertical);
  static std::vector<Rectangle>
  findSpreadsheetsFromCells(const std::vector<Rectangle> &cells);
};

} // namespace tabula
