#pragma once
#ifdef HAVE_POPPLER
#include "tabula/ObjectExtractorStreamEngine.h"
#include <poppler/cpp/poppler-page.h>
#include <vector>

namespace tabula {

// Engine that analyzes a poppler::page raster image to find long
// horizontal/vertical strokes and converts them to Ruling objects.
class ObjectExtractorPopplerEngine : public ObjectExtractorStreamEngine {
public:
  ObjectExtractorPopplerEngine() {}
  ~ObjectExtractorPopplerEngine() {}

  // analyze the poppler page and populate rulings via addSegment
  void analyze(poppler::page &p);
};

} // namespace tabula
#endif
