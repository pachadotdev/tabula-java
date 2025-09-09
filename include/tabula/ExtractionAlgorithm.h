#pragma once
#include <vector>

namespace tabula {
class Page;
class Table;

class ExtractionAlgorithm {
public:
  virtual ~ExtractionAlgorithm() {}
  virtual std::vector<Table> extract(const Page &page) const = 0;
};

} // namespace tabula
