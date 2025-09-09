#include "tabula/Cell.h"
#include "tabula/Rectangle.h"
#include "tabula/TableWithRulingLines.h"
#include <iostream>

int main() {
  using namespace tabula;
  Rectangle area{0, 0, 200, 200};
  std::vector<Cell> cells;
  // create 4 cells forming 2 rows x 2 cols (Cell constructor:
  // top,left,width,height)
  cells.emplace_back(0.0f, 0.0f, 50.0f, 50.0f);
  cells.emplace_back(0.0f, 50.0f, 50.0f, 50.0f);
  cells.emplace_back(50.0f, 0.0f, 50.0f, 50.0f);
  cells.emplace_back(50.0f, 50.0f, 50.0f, 50.0f);

  TableWithRulingLines tw(area, cells, std::vector<tabula::Ruling>{},
                          std::vector<tabula::Ruling>{}, 0);
  if (tw.getRows().size() != 2) {
    std::cerr << "Expected 2 rows, got " << tw.getRows().size() << std::endl;
    return 1;
  }
  std::cout << "tablewithrulings OK" << std::endl;
  return 0;
}
