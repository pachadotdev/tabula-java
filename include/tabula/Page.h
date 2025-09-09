#pragma once
#include "tabula/Rectangle.h"
#include "tabula/Ruling.h"
#include "tabula/TextChunk.h"
#include "tabula/Utils.h"
#include <stdexcept>
#include <vector>

namespace tabula {

class Page : public Rectangle {
public:
  Page() : Rectangle(), minCharWidth(0), minCharHeight(0), pageNumber(0) {}
  explicit Page(int number)
      : Rectangle(), minCharWidth(0), minCharHeight(0), pageNumber(number) {}
  void addTextChunk(const TextChunk &tc) { textChunks.push_back(tc); }
  const std::vector<TextChunk> &getTextChunks() const { return textChunks; }

  const std::vector<Ruling> &getRulings() const {
    if (!cleanRulings.empty())
      return cleanRulings;

    if (rulings.empty()) {
      verticalRulingLines.clear();
      horizontalRulingLines.clear();
      return cleanRulings;
    }

    // snap points
    // make a mutable copy for snapping
    std::vector<Ruling> temp = rulings;
    Utils::snapPoints(temp, minCharWidth, minCharHeight);

    // separate vertical/horizontal
    verticalRulingLines = getCollapsedVerticalRulings(temp);
    horizontalRulingLines = getCollapsedHorizontalRulings(temp);

    cleanRulings = verticalRulingLines;
    cleanRulings.insert(cleanRulings.end(), horizontalRulingLines.begin(),
                        horizontalRulingLines.end());
    return cleanRulings;
  }

  std::vector<Ruling>
  getCollapsedVerticalRulings(const std::vector<Ruling> &source) const {
    std::vector<Ruling> verticalRulings;
    for (const auto &r : source)
      if (r.vertical())
        verticalRulings.push_back(r);
    return Ruling::collapseOrientedRulings(verticalRulings);
  }

  std::vector<Ruling>
  getCollapsedHorizontalRulings(const std::vector<Ruling> &source) const {
    std::vector<Ruling> horizontalRulings;
    for (const auto &r : source)
      if (r.horizontal())
        horizontalRulings.push_back(r);
    return Ruling::collapseOrientedRulings(horizontalRulings);
  }

  void addRuling(const Ruling &r) {
    // Normalize a copy of the incoming ruling and accept it if it becomes
    // vertical/horizontal.
    Ruling nr = r;
    // Ignore zero-length rulings
    if (nr.length() == 0)
      return;
    nr.normalize();
    if (nr.oblique()) {
      // Try a wider tolerance: if angle is within 5 degrees of axis, force it
      try {
        double angle = nr.getAngle();
        if (Utils::within(angle, 0, 5) || Utils::within(angle, 180, 5)) {
          nr.y2 = nr.y1; // force horizontal
        } else if (Utils::within(angle, 90, 5) ||
                   Utils::within(angle, 270, 5)) {
          nr.x2 = nr.x1; // force vertical
        } else {
          // As a last resort, skip the oblique ruling silently
          return;
        }
      } catch (...) {
        // If angle can't be determined, skip the ruling
        return;
      }
    }
    rulings.push_back(nr);
    verticalRulingLines.clear();
    horizontalRulingLines.clear();
    cleanRulings.clear();
  }

  const std::vector<Ruling> &getUnprocessedRulings() const { return rulings; }

  void setMinCharDims(float w, float h) {
    minCharWidth = w;
    minCharHeight = h;
  }

  // Convenience: run projection-profile separators using this page's text
  // elements
  std::vector<float> findVerticalSeparators(float minColumnWidth) const;
  std::vector<float> findHorizontalSeparators(float minRowHeight) const;

  void setPageNumber(int n) { pageNumber = n; }
  int getPageNumber() const { return pageNumber; }

private:
  std::vector<TextChunk> textChunks;
  std::vector<Ruling> rulings;
  mutable std::vector<Ruling> cleanRulings;
  mutable std::vector<Ruling> verticalRulingLines;
  mutable std::vector<Ruling> horizontalRulingLines;
  float minCharWidth;
  float minCharHeight;
  int pageNumber;
};

} // namespace tabula
