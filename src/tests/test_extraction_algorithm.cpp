#include "tabula/BasicExtractionAlgorithm.h"
#include "tabula/Page.h"
#include "tabula/Ruling.h"
#include <iostream>

int main() {
  using namespace tabula;
  Page p;
  p.setTop(0);
  p.setLeft(0);
  p.setRight(100);
  p.setBottom(100);
  // vertical rulings at x=0,50,100
  p.addRuling(Ruling(0, 0, 0, 100));
  p.addRuling(Ruling(50, 0, 50, 100));
  p.addRuling(Ruling(100, 0, 100, 100));
  // horizontal rulings at y=0,50,100
  p.addRuling(Ruling(0, 0, 100, 0));
  p.addRuling(Ruling(0, 50, 100, 50));
  p.addRuling(Ruling(0, 100, 100, 100));

  BasicExtractionAlgorithm bea;
  auto tables = bea.extract(p, 10.0f, 10.0f);
  if (tables.empty())
    return 2;
  auto &t = tables.front();
  if (t.getRows().size() != 2)
    return 3;
  std::cout << "extraction_algorithm OK\n";
  return 0;
}
