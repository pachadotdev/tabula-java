#include "tabula/ObjectExtractorStreamEngine.h"
#include <iostream>

int main() {
  using namespace tabula;
  ObjectExtractorStreamEngine eng(0.5f);
  eng.addSegment(0.0f, 0.0f, 1.0f, 0.0f); // length 1.0 -> kept
  eng.addSegment(0.0f, 0.0f, 0.1f, 0.0f); // length 0.1 -> discarded
  auto &r = eng.getRulings();
  if (r.size() != 1)
    return 2;
  std::cout << "streamengine OK\n";
  return 0;
}
