#include "tabula/TextChunk.h"
#include "tabula/Ruling.h"
#include <algorithm>
#include <cmath>

namespace tabula {

std::vector<TextChunk>
TextChunk::mergeWords(const std::vector<TextElement> &elements,
                      const std::vector<Ruling> *verticalRulings) {
  std::vector<TextChunk> out;
  if (elements.empty())
    return out;

  struct E {
    TextElement te;
    float centerY;
  };
  std::vector<E> es;
  es.reserve(elements.size());
  for (auto const &te : elements)
    es.push_back({te, te.getTop() + te.getHeight() * 0.5f});

  std::sort(es.begin(), es.end(), [](const E &a, const E &b) {
    if (a.centerY != b.centerY)
      return a.centerY < b.centerY;
    return a.te.getLeft() < b.te.getLeft();
  });

  // Determine a line clustering threshold based on average element height to
  // adapt to different font sizes; fall back to a small constant if needed.
  float avgHeight = 0.0f;
  for (auto const &e : elements)
    avgHeight += e.getHeight();
  avgHeight =
      (elements.empty() ? 0.0f
                        : avgHeight / static_cast<float>(elements.size()));
  float lineThreshold = std::max(1.0f, avgHeight * 0.6f);
  std::vector<std::vector<TextElement>> lines;
  for (auto &e : es) {
    if (lines.empty()) {
      lines.emplace_back();
      lines.back().push_back(e.te);
      continue;
    }
    float prevCenter =
        lines.back()[0].getTop() + lines.back()[0].getHeight() * 0.5f;
    if (std::abs(e.centerY - prevCenter) <= lineThreshold)
      lines.back().push_back(e.te);
    else {
      lines.emplace_back();
      lines.back().push_back(e.te);
    }
  }

  auto hasVerticalRulingBetween = [&](double leftEdge, double rightEdge,
                                      double topY, double bottomY) {
    if (!verticalRulings)
      return false;
    for (auto const &vr : *verticalRulings) {
      if (!vr.vertical())
        continue;
      double x = vr.getLeft();
      if (x <= leftEdge || x >= rightEdge)
        continue;
      if (!(vr.getBottom() < topY || vr.getTop() > bottomY))
        return true;
    }
    return false;
  };

  for (auto &line : lines) {
    if (line.empty())
      continue;
    std::sort(line.begin(), line.end(),
              [](const TextElement &a, const TextElement &b) {
                return a.getLeft() < b.getLeft();
              });

    std::vector<std::vector<TextElement>> words;
    words.emplace_back();
    words.back().push_back(line.front());

    for (size_t i = 1; i < line.size(); ++i) {
      const TextElement &prev = line[i - 1];
      const TextElement &cur = line[i];
      double gap = cur.getLeft() - (prev.getLeft() + prev.getWidth());
      float spaceWidth =
          std::max(prev.getWidthOfSpace(), cur.getWidthOfSpace());
      if (!std::isfinite(spaceWidth) || spaceWidth <= 0)
        spaceWidth = 1.0f;
      bool splitOnGap = gap > spaceWidth * 1.5;
      bool splitOnRuling = hasVerticalRulingBetween(
          prev.getLeft() + prev.getWidth(), cur.getLeft(),
          std::min(prev.getTop(), cur.getTop()),
          std::max(prev.getBottom(), cur.getBottom()));
      if (splitOnGap || splitOnRuling)
        words.emplace_back();
      words.back().push_back(cur);
    }

    for (auto &w : words) {
      if (w.empty())
        continue;
      float top = w.front().getTop();
      float left = w.front().getLeft();
      float right = w.front().getLeft() + w.front().getWidth();
      float bottom = w.front().getTop() + w.front().getHeight();
      std::string s;
      for (size_t i = 0; i < w.size(); ++i) {
        top = std::min(top, w[i].getTop());
        left = std::min(left, w[i].getLeft());
        right = std::max(right, w[i].getLeft() + w[i].getWidth());
        bottom = std::max(bottom, w[i].getTop() + w[i].getHeight());
        s += w[i].getText();
        if (i + 1 < w.size())
          s.push_back(' ');
      }
      TextChunk tc(top, left, right - left, bottom - top);
      TextElement te(top, left, right - left, bottom - top, s, 0.0f);
      tc.add(te);
      out.push_back(tc);
    }
  }

  return out;
}

} // namespace tabula
