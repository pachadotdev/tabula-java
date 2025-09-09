#include "tabula/TextStripper.h"
#include <algorithm>

namespace tabula {

std::vector<TextChunk>
TextStripper::strip(const std::vector<TextElement> &elements) const {
  std::vector<TextChunk> out;
  if (elements.empty())
    return out;

  // Copy elements and sort by vertical center
  struct E {
    TextElement te;
    float centerY;
  };
  std::vector<E> es;
  es.reserve(elements.size());
  for (auto const &te : elements)
    es.push_back({te, te.getTop() + te.getHeight() * 0.5f});
  std::sort(es.begin(), es.end(),
            [](const E &a, const E &b) { return a.centerY < b.centerY; });

  // group into lines
  std::vector<std::vector<TextElement>> lines;
  for (auto &e : es) {
    if (lines.empty()) {
      lines.emplace_back();
      lines.back().push_back(e.te);
      continue;
    }
    float prevCenter =
        lines.back()[0].getTop() + lines.back()[0].getHeight() * 0.5f;
    if (std::abs(e.centerY - prevCenter) <= lineThreshold_) {
      lines.back().push_back(e.te);
    } else {
      lines.emplace_back();
      lines.back().push_back(e.te);
    }
  }

  // build TextChunks: sort each line by left coordinate
  for (auto &line : lines) {
    std::sort(line.begin(), line.end(),
              [](const TextElement &a, const TextElement &b) {
                return a.getLeft() < b.getLeft();
              });
    // merge into single TextChunk
    float top = line.front().getTop();
    float left = line.front().getLeft();
    float right = line.front().getLeft() + line.front().getWidth();
    float bottom = top + line.front().getHeight();
    for (size_t i = 1; i < line.size(); ++i) {
      top = std::min(top, line[i].getTop());
      left = std::min(left, line[i].getLeft());
      right = std::max(right, line[i].getLeft() + line[i].getWidth());
      bottom = std::max(bottom, line[i].getTop() + line[i].getHeight());
    }
    TextChunk tc(top, left, right - left, bottom - top);
    for (auto &te : line)
      tc.add(te);
    out.push_back(tc);
  }

  return out;
}

} // namespace tabula
