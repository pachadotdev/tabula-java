#pragma once
#include "tabula/Rectangle.h"
#include "tabula/TextChunk.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace tabula {

struct Line : public Rectangle {
  std::vector<TextChunk> textChunks;
  double x1{0}, y1{0}, x2{0}, y2{0};
  Line() : Rectangle(), textChunks() {}
  Line(double x1_, double y1_, double x2_, double y2_)
      : Rectangle(std::min((float)y1_, (float)y2_),
                  std::min((float)x1_, (float)x2_),
                  std::abs((float)(x2_ - x1_)), std::abs((float)(y2_ - y1_))),
        x1(x1_), y1(y1_), x2(x2_), y2(y2_) {}

  double length() const { return std::hypot(x2 - x1, y2 - y1); }
  bool isHorizontal(double tolerance = 0.01) const {
    return std::abs(y1 - y2) < tolerance;
  }
  bool isVertical(double tolerance = 0.01) const {
    return std::abs(x1 - x2) < tolerance;
  }

  void addTextChunk(int i, const TextChunk &textChunk) {
    if (i < 0)
      throw std::invalid_argument("i can't be less than 0");
    if (static_cast<int>(textChunks.size()) < i + 1) {
      textChunks.resize(i + 1);
      textChunks[i] = textChunk;
    } else {
      // merge semantics: merge geometries and text elements
      textChunks[i].merge(textChunk);
    }
    // expand this line's geometry to include the chunk
    Rectangle::merge(textChunk);
    // update coordinate endpoints used by Ruling/Line math
    x1 = std::min(x1, static_cast<double>(textChunk.getLeft()));
    y1 = std::min(y1, static_cast<double>(textChunk.getTop()));
    x2 = std::max(x2, static_cast<double>(textChunk.getRight()));
    y2 = std::max(y2, static_cast<double>(textChunk.getBottom()));
  }

  void addTextChunk(const TextChunk &textChunk) {
    // always expand this line's geometry
    Rectangle::merge(textChunk);
    // update endpoints
    if (textChunks.empty()) {
      x1 = textChunk.getLeft();
      y1 = textChunk.getTop();
      x2 = textChunk.getRight();
      y2 = textChunk.getBottom();
    } else {
      x1 = std::min(x1, static_cast<double>(textChunk.getLeft()));
      y1 = std::min(y1, static_cast<double>(textChunk.getTop()));
      x2 = std::max(x2, static_cast<double>(textChunk.getRight()));
      y2 = std::max(y2, static_cast<double>(textChunk.getBottom()));
    }
    textChunks.push_back(textChunk);
  }

  const std::vector<TextChunk> &getTextElements() const { return textChunks; }

  static Line removeRepeatedCharacters(const Line &line, char c,
                                       int minRunLength) {
    Line rv;
    for (const auto &t : line.getTextElements()) {
      auto parts = t.squeeze(c, minRunLength);
      for (const auto &r : parts)
        rv.addTextChunk(r);
    }
    return rv;
  }
};

} // namespace tabula
