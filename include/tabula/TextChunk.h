#pragma once
#include "tabula/RectangularTextContainer.h"
#include "tabula/TextElement.h"
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace tabula {
struct Ruling;
}

namespace tabula {

class TextChunk : public RectangularTextContainer<TextElement> {
public:
  TextChunk() : RectangularTextContainer(0, 0, 0, 0) {}
  TextChunk(float top, float left, float width, float height)
      : RectangularTextContainer(top, left, width, height) {}
  TextChunk(TextElement const &te)
      : RectangularTextContainer(te.getTop(), te.getLeft(), te.getWidth(),
                                 te.getHeight()) {
    this->add(te);
  }
  // convenience: construct from raw text and position
  TextChunk(const std::string &txt, float top, float left)
      : RectangularTextContainer(top, left, 0, 0) {
    TextElement te(top, left, 0, 0, txt, 0.0f, 0.0f);
    this->add(te);
  }

  std::string getText() const override {
    if (textElements.empty())
      return std::string();
    std::string sb;
    for (auto const &te : textElements)
      sb += te.getText();
    return sb;
  }
  std::string getText(bool) const override { return getText(); }

  void add(TextElement const &te) {
    textElements.push_back(te);
    this->merge(te);
  }

  // squeeze: split runs of char c of length >= minRunLength into boundaries
  // Produces TextChunks for regions between long runs of `c`. This approximates
  // per-character bounding boxes by dividing each TextElement width by its
  // character count.
  std::vector<TextChunk> squeeze(char c = ' ', int minRunLength = 2) const {
    std::vector<TextChunk> out;
    if (textElements.empty())
      return out;

    struct CharInfo {
      char ch;
      float top;
      float left;
      float width;
      float height;
    };
    std::vector<CharInfo> chars;
    chars.reserve(256);

    for (const auto &te : textElements) {
      std::string txt = te.getText();
      int n = (int)txt.size();
      float elemWidth = te.getWidth();
      float charW = (n > 0) ? (elemWidth / static_cast<float>(std::max(1, n)))
                            : te.getWidthOfSpace();
      if (!std::isfinite(charW) || charW <= 0)
        charW = 1.0f;
      for (int i = 0; i < n; ++i) {
        CharInfo ci;
        ci.ch = txt[i];
        ci.top = te.getTop();
        ci.left = te.getLeft() + i * charW;
        ci.width = charW;
        ci.height = te.getHeight();
        chars.push_back(ci);
      }
    }

    if (chars.empty()) {
      out.push_back(*this);
      return out;
    }

    std::vector<std::pair<int, int>> runs;
    int i = 0;
    while (i < (int)chars.size()) {
      if (chars[i].ch == c) {
        int j = i;
        while (j < (int)chars.size() && chars[j].ch == c)
          ++j;
        if (j - i >= minRunLength)
          runs.emplace_back(i, j);
        i = j;
      } else
        ++i;
    }

    if (runs.empty()) {
      out.push_back(*this);
      return out;
    }

    int segStart = 0;
    for (auto &r : runs) {
      int runStart = r.first;
      if (segStart < runStart) {
        int a = segStart, b = runStart;
        float left = chars[a].left;
        float right = chars[b - 1].left + chars[b - 1].width;
        float top = chars[a].top;
        float bottom = chars[a].top + chars[a].height;
        for (int k = a + 1; k < b; ++k) {
          top = std::min(top, chars[k].top);
          bottom = std::max(bottom, chars[k].top + chars[k].height);
        }
        TextChunk tc(top, left, right - left, bottom - top);
        std::string s;
        s.reserve(b - a);
        for (int k = a; k < b; ++k)
          s.push_back(chars[k].ch);
        TextElement te(top, left, right - left, bottom - top, s, 0.0f, 0.0f);
        tc.add(te);
        out.push_back(tc);
      }
      segStart = r.second;
    }

    if (segStart < (int)chars.size()) {
      int a = segStart, b = (int)chars.size();
      float left = chars[a].left;
      float right = chars[b - 1].left + chars[b - 1].width;
      float top = chars[a].top;
      float bottom = chars[a].top + chars[a].height;
      for (int k = a + 1; k < b; ++k) {
        top = std::min(top, chars[k].top);
        bottom = std::max(bottom, chars[k].top + chars[k].height);
      }
      TextChunk tc(top, left, right - left, bottom - top);
      std::string s;
      s.reserve(b - a);
      for (int k = a; k < b; ++k)
        s.push_back(chars[k].ch);
      TextElement te(top, left, right - left, bottom - top, s, 0.0f, 0.0f);
      tc.add(te);
      out.push_back(tc);
    }

    return out;
  }

  // Merge a flat list of TextElements into word-level TextChunks.
  // Implemented in cpp/src/TextChunk.cpp. Accept a pointer to rulings to avoid
  // instantiating std::vector<Ruling> in this header (Ruling is
  // forward-declared).
  static std::vector<TextChunk>
  mergeWords(const std::vector<TextElement> &elements,
             const std::vector<Ruling> *verticalRulings = nullptr);
};

} // namespace tabula
