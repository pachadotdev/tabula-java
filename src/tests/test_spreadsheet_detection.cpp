#include "tabula/Page.h"
#include "tabula/Ruling.h"
#include "tabula/detectors/SpreadsheetDetectionAlgorithm.h"
#include <iostream>

int main() {
  using namespace tabula;
  using namespace tabula::detectors;
  Page page;
  // Add a tiny grid
  page.addRuling(Ruling(0, 0, 100, 0));
  page.addRuling(Ruling(0, 50, 100, 50));
  page.addRuling(Ruling(0, 0, 0, 50));
  page.addRuling(Ruling(100, 0, 100, 50));

  SpreadsheetDetectionAlgorithm det;
  auto tables = det.detect(page);
  std::cout << "detected=" << tables.size() << "\n";
  return tables.empty() ? 2 : 0;
}
