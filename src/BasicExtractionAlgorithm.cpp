#include "tabula/BasicExtractionAlgorithm.h"
#include "tabula/Cell.h"
#include "tabula/TableDetector.h"
#include "tabula/TextChunk.h"
#include <algorithm>
#include <map>
#include <vector>

namespace tabula {

std::vector<Table> BasicExtractionAlgorithm::extract(const Page &page,
                                                     float minColumnWidth,
                                                     float minRowHeight) const {
  std::vector<Rectangle> rects =
      TableDetector::detectCells(page, minColumnWidth, minRowHeight);
  // Build a simple table: convert rectangles into rows by y coordinate
  if (rects.empty())
    return std::vector<Table>();

  // group by rows (top coordinate)
  std::map<float, std::vector<Rectangle>> rows;
  for (auto const &r : rects)
    rows[r.getTop()].push_back(r);

  Table table;
  for (auto const &pr : rows) {
    std::vector<Cell> rowCells;
    for (auto const &r : pr.second) {
      Cell c(r.getTop(), r.getLeft(), r.getWidth(), r.getHeight());
      // Collect raw TextElements from the page that overlap this cell and
      // merge into word-level TextChunks (mirror Java per-cell merge)
      try {
        std::vector<TextElement> elems;
        for (const auto &tc : page.getTextChunks()) {
          for (const auto &te : tc.getTextElements()) {
            if (te.overlapRatio(r) > 0.0f || r.overlapRatio(te) > 0.0f)
              elems.push_back(te);
          }
        }
        if (!elems.empty()) {
          auto merged = TextChunk::mergeWords(elems, nullptr);
          c.setTextElements(merged);
        }
      } catch (...) {}
      rowCells.push_back(c);
    }
    table.addRow(rowCells);
  }

  return std::vector<Table>{table};
}

} // namespace tabula
