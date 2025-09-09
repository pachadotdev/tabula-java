#include "tabula/TableDetector.h"
#include "tabula/ProjectionProfile.h"

namespace tabula {

std::vector<Rectangle> TableDetector::detectCells(const Page &page,
                                                  float minColumnWidth,
                                                  float minRowHeight) {
  // get separators (including page borders)
  std::vector<float> vSeps = page.findVerticalSeparators(minColumnWidth);
  std::vector<float> hSeps = page.findHorizontalSeparators(minRowHeight);

  // ensure borders are included
  float left = page.getLeft();
  float right = page.getRight();
  float top = page.getTop();
  float bottom = page.getBottom();
  auto push_border = [&](std::vector<float> &vec, float border) {
    if (vec.empty() || std::abs(vec.front() - border) > 1e-6)
      vec.insert(vec.begin(), border);
  };
  auto push_back_border = [&](std::vector<float> &vec, float border) {
    if (vec.empty() || std::abs(vec.back() - border) > 1e-6)
      vec.push_back(border);
  };
  push_border(vSeps, left);
  push_back_border(vSeps, right);
  push_border(hSeps, top);
  push_back_border(hSeps, bottom);

  std::vector<Rectangle> cells;
  for (size_t i = 0; i + 1 < hSeps.size(); ++i) {
    for (size_t j = 0; j + 1 < vSeps.size(); ++j) {
      float t = hSeps[i];
      float b = hSeps[i + 1];
      float l = vSeps[j];
      float r = vSeps[j + 1];
      cells.emplace_back(t, l, r - l, b - t);
    }
  }
  return cells;
}

} // namespace tabula
