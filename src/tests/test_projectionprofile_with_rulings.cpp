#include "tabula/Page.h"
#include "tabula/ProjectionProfile.h"
#include "tabula/Ruling.h"
#include <cassert>
#include <iostream>

using namespace tabula;

int main() {
  Page p;
  // page area
  p.setLeft(0);
  p.setTop(0);
  p.setRight(100);
  p.setBottom(100);
  // add explicit vertical rulings at x=10 and x=50
  p.addRuling(Ruling(10, 0, 10, 100));
  p.addRuling(Ruling(50, 0, 50, 100));

  // no text chunks needed for this test
  p.setMinCharDims(1.0f, 1.0f);

  auto seps = p.findVerticalSeparators(2.0f);
  // Expect that explicit rulings are present (approx 2 separators)
  assert(seps.size() >= 2);
  std::cout << "test_projectionprofile_with_rulings: OK (seps=" << seps.size()
            << ")\n";
  return 0;
}
