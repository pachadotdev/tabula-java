#include "tabula/Page.h"
#include "tabula/Ruling.h"
#include "tabula/TableDetector.h"
#include <cassert>
#include <iostream>

using namespace tabula;

int main() {
  Page p;
  p.setLeft(0);
  p.setTop(0);
  p.setRight(100);
  p.setBottom(100);
  // Add vertical rulings at 0, 50, 100 and horizontal at 0, 50, 100
  p.addRuling(Ruling(0, 0, 0, 100));
  p.addRuling(Ruling(50, 0, 50, 100));
  p.addRuling(Ruling(100, 0, 100, 100));
  p.addRuling(Ruling(0, 0, 100, 0));
  p.addRuling(Ruling(0, 50, 100, 50));
  p.addRuling(Ruling(0, 100, 100, 100));

  auto cells = TableDetector::detectCells(p, 1.0f, 1.0f);
  // expect 2x2 grid -> 4 cells
  assert(cells.size() == 4);
  std::cout << "test_table_detector: OK (cells=" << cells.size() << ")\n";
  return 0;
}
