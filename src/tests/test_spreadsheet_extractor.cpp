#include "tabula/Ruling.h"
#include "tabula/SpreadsheetExtractionAlgorithm.h"
#include <cassert>
#include <iostream>

int main() {
  using namespace tabula;
  try {
    // Create a simple 2x2 grid: verticals at x=0 and x=100, horizontals at y=0
    // and y=50
    std::vector<Ruling> hs = {Ruling(0, 0, 100, 0), Ruling(0, 50, 100, 50)};
    std::vector<Ruling> vs = {Ruling(0, 0, 0, 50), Ruling(100, 0, 100, 50)};
    auto cells = tabula::SpreadsheetExtractionAlgorithm::findCells(hs, vs);
    if (cells.empty()) {
      std::cerr << "No cells found\n";
      return 2;
    }
    std::cout << "spreadsheet extractor OK\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "EXCEPTION: " << e.what() << "\n";
    return 3;
  }
}
