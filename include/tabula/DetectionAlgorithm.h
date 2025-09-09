#pragma once
#include <vector>

namespace tabula {
class Page;
class Rectangle;

class DetectionAlgorithm {
public:
  virtual ~DetectionAlgorithm() {}
  virtual std::vector<Rectangle> detect(const Page &page) = 0;
};

} // namespace tabula
