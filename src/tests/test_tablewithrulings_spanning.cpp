#include "tabula/Cell.h"
#include "tabula/Rectangle.h"
#include "tabula/Ruling.h"
#include "tabula/TableWithRulingLines.h"
#include <iostream>

int main() {
  using namespace tabula;
  Rectangle area{0, 0, 200, 100};
  // vertical rulings at x=0, 50, 100, 200 define 3 columns
  // (0-50,50-100,100-200)
  std::vector<Ruling> vr;
  vr.emplace_back(0.0, 0.0, 0.0, 100.0);
  vr.emplace_back(50.0, 0.0, 50.0, 100.0);
  vr.emplace_back(100.0, 0.0, 100.0, 100.0);
  vr.emplace_back(200.0, 0.0, 200.0, 100.0);

  // single row with one cell spanning from left=0 to right=100 (covers first
  // two columns)
  std::vector<Cell> cells;
  cells.emplace_back(0.0f, 0.0f, 100.0f, 20.0f);

  TableWithRulingLines t(area, cells, std::vector<Ruling>{}, vr, 0);
  auto rows = t.getRows();
  if (rows.size() != 1) {
    std::cerr << "Expected 1 row, got " << rows.size() << std::endl;
    return 1;
  }
  if (rows[0].size() < 2) {
    std::cerr << "Expected at least 2 columns, got " << rows[0].size()
              << std::endl;
    return 1;
  }
  // main cell should be in column 0 and marked spanning
  if (!rows[0][0].isSpanning()) {
    std::cerr << "Expected first cell to be spanning" << std::endl;
    return 1;
  }
  // placeholder should be at column 1
  if (!rows[0][1].isPlaceholder()) {
    std::cerr << "Expected placeholder at column 1" << std::endl;
    return 1;
  }

  std::cout << "tablewithrulings_spanning OK" << std::endl;
  return 0;
}
