#pragma once
#include "tabula/ObjectExtractor.h"
#include "tabula/Page.h"
#include <iterator>
#include <vector>

namespace tabula {

class PageIterator {
public:
  PageIterator(ObjectExtractor *extractor, const std::vector<int> &pages)
      : extractor_(extractor), pages_(pages), idx_(0) {}

  bool hasNext() const { return idx_ < pages_.size(); }
  Page next() {
    if (!hasNext())
      throw std::out_of_range("no next page");
    return extractor_->extractPage(pages_[idx_++]);
  }

private:
  ObjectExtractor *extractor_;
  std::vector<int> pages_;
  size_t idx_;
};

} // namespace tabula
