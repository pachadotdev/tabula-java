#include "tabula/Cell.h"
#include "tabula/Rectangle.h"
#include "tabula/Ruling.h"
#include "tabula/TableWithRulingLines.h"
#include <iostream>

int main() {
  using namespace tabula;
  Rectangle area{0, 0, 200, 200};
  // horizontal rulings at y=0, 50, 100, 200 define 3 rows
  std::vector<Ruling> hr;
  hr.emplace_back(0.0, 0.0, 200.0, 0.0);
  hr.emplace_back(0.0, 50.0, 200.0, 50.0);
  hr.emplace_back(0.0, 100.0, 200.0, 100.0);
  hr.emplace_back(0.0, 200.0, 200.0, 200.0);

  // single column with one cell spanning from top=0 to bottom=100 (covers first
  // two rows)
  std::vector<Cell> cells;
  cells.emplace_back(0.0f, 0.0f, 50.0f, 100.0f);

  TableWithRulingLines t(area, cells, hr, std::vector<Ruling>{}, 0);
  auto rows = t.getRows();
  if (rows.size() < 2) {
    std::cerr << "Expected at least 2 rows, got " << rows.size() << std::endl;
    return 1;
  }
  // main cell should be at (0,0) and marked spanning
  if (!rows[0][0].isSpanning()) {
    std::cerr << "Expected first cell to be spanning" << std::endl;
    return 1;
  }
  // placeholder should be at (1,0)
  if (!rows[1][0].isPlaceholder()) {
    std::cerr << "Expected placeholder at (1,0)" << std::endl;
    return 1;
  }

  std::cout << "tablewithrulings_rowspan OK" << std::endl;
  return 0;
}
