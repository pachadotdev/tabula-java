#include "tabula/Page.h"
#include "tabula/ProjectionProfile.h"

namespace tabula {

std::vector<float> Page::findVerticalSeparators(float minColumnWidth) const {
  // Build rectangles from text chunks
  std::vector<Rectangle> elems;
  for (const auto &tc : textChunks)
    elems.push_back(tc);

  ProjectionProfile pp(*this, elems, 3.0f, 3.0f);
  // pass explicit collapsed vertical rulings
  auto explicitR = getCollapsedVerticalRulings(getUnprocessedRulings());
  return pp.findVerticalSeparators(minColumnWidth, explicitR);
}

std::vector<float> Page::findHorizontalSeparators(float minRowHeight) const {
  std::vector<Rectangle> elems;
  for (const auto &tc : textChunks)
    elems.push_back(tc);

  ProjectionProfile pp(*this, elems, 3.0f, 3.0f);
  auto explicitR = getCollapsedHorizontalRulings(getUnprocessedRulings());
  return pp.findHorizontalSeparators(minRowHeight, explicitR);
}

} // namespace tabula
