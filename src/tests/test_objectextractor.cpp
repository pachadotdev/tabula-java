#include "tabula/ObjectExtractor.h"
#include "tabula/ObjectExtractorStreamEngine.h"
#include "tabula/TextElement.h"
#include <iostream>

int main() {
  using namespace tabula;
  ObjectExtractorStreamEngine eng(0.1f);
  eng.addSegment(0.0f, 0.0f, 2.0f, 0.0f);

  std::vector<TextElement> elems;
  elems.emplace_back(0.0f, 0.0f, 3.0f, 5.0f, "Hi", 0.0f);

  ObjectExtractor oe(&eng, elems);
  Page p = oe.extractPage(1);
  if (p.getUnprocessedRulings().size() != 1)
    return 2;
  if (p.getTextChunks().size() != 1)
    return 3;
  std::cout << "objectextractor OK\n";
  return 0;
}
