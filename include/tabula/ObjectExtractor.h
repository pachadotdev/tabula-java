#pragma once
#include "tabula/ObjectExtractorStreamEngine.h"
#include "tabula/Page.h"
#include "tabula/TextElement.h"
#include "tabula/TextStripper.h"
#include <vector>

// Simple test-oriented ObjectExtractor: holds an engine and a set of text
// elements that it will use to populate Pages.

namespace tabula {

// Minimal ObjectExtractor: in the Java project this wraps a PDDocument.
// Here it's a stub that can be used by tests; extractPage returns a Page
// with the requested number and empty content.
class ObjectExtractor {
public:
  ObjectExtractor() : engine_(nullptr), stripper_(2.0f) {}
  ObjectExtractor(ObjectExtractorStreamEngine *engine,
                  const std::vector<TextElement> &elements)
      : engine_(engine), stripper_(2.0f), elements_(elements) {}
  virtual ~ObjectExtractor() {}

  virtual Page extractPage(int pageNumber);

  void setEngine(ObjectExtractorStreamEngine *e) { engine_ = e; }
  void setTextElements(const std::vector<TextElement> &e) { elements_ = e; }

private:
  ObjectExtractorStreamEngine *engine_;
  TextStripper stripper_;
  std::vector<TextElement> elements_;
};

} // namespace tabula
