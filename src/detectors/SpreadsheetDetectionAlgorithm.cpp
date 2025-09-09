#include "tabula/detectors/SpreadsheetDetectionAlgorithm.h"
#include "tabula/SpreadsheetExtractionAlgorithm.h"

namespace tabula {
namespace detectors {

std::vector<Rectangle>
SpreadsheetDetectionAlgorithm::detect(const Page &page) const {
  std::vector<Cell> cells = SpreadsheetExtractionAlgorithm::findCells(
      page.getCollapsedHorizontalRulings(page.getUnprocessedRulings()),
      page.getCollapsedVerticalRulings(page.getUnprocessedRulings()));
  std::vector<Rectangle> tables =
      SpreadsheetExtractionAlgorithm::findSpreadsheetsFromCells(
          std::vector<Rectangle>(cells.begin(), cells.end()));
  return tables;
}

} // namespace detectors
} // namespace tabula
