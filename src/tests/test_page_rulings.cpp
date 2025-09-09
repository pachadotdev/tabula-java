#include "tabula/Page.h"
#include "tabula/Ruling.h"
#include <cassert>
#include <iostream>

using namespace tabula;

int main() {
  Page p;
  // Add a few vertical and horizontal rulings that should collapse
  p.setMinCharDims(1.0f, 1.0f);
  // vertical rulings at x=10 (two close ones), x=50
  p.addRuling(Ruling(10, 0, 10, 100));
  p.addRuling(Ruling(11, 0, 11, 100));
  p.addRuling(Ruling(50, 0, 50, 100));
  // horizontal rulings at y=20 (two close ones), y=80
  p.addRuling(Ruling(0, 20, 100, 20));
  p.addRuling(Ruling(0, 21, 100, 21));
  p.addRuling(Ruling(0, 80, 100, 80));

  auto rulings = p.getRulings();
  // should have collapsed some: expect at least 4 (2 vertical + 2 horizontal)
  assert(rulings.size() >= 4);
  std::cout << "test_page_rulings: OK (rulings=" << rulings.size() << ")\n";
  return 0;
}
