
#include "tabula/ObjectExtractor.h"
#include "tabula/TextChunk.h"

namespace tabula {

Page ObjectExtractor::extractPage(int pageNumber) {
  Page p(pageNumber);

  // If an engine is provided, copy rulings
  if (engine_) {
    for (const auto &r : engine_->getRulings())
      p.addRuling(r);
  }

  // Use stripper to group elements into chunks
  auto chunks = stripper_.strip(elements_);
  for (const auto &tc : chunks)
    p.addTextChunk(tc);

  // set min char dims heuristically from elements if available
  if (!elements_.empty()) {
    float minW = 0.0f, minH = 0.0f;
    for (const auto &e : elements_) {
      if (minW == 0.0f || e.getWidth() < minW)
        minW = e.getWidth();
      if (minH == 0.0f || e.getHeight() < minH)
        minH = e.getHeight();
    }
    p.setMinCharDims(minW, minH);
  }

  return p;
}

} // namespace tabula
