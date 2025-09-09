#pragma once
#include "tabula/RectangularTextContainer.h"
#include "tabula/TextChunk.h"
#include <algorithm>
#include <string>
#include <vector>

namespace tabula {

class Cell : public RectangularTextContainer<TextChunk> {
public:
  Cell(float top, float left, float width, float height)
      : RectangularTextContainer<TextChunk>(top, left, width, height),
        spanning(false), placeholder(false) {}
  Cell()
      : RectangularTextContainer<TextChunk>(), spanning(false),
        placeholder(false) {}

  void addTextChunk(const TextChunk &tc) { this->textElements.push_back(tc); }

  // port of getText(useLineReturns=true) from Java
  std::string getText(bool useLineReturns) const {
    if (this->textElements.empty())
      return std::string();
    // work on a local copy to preserve const-correctness
    std::vector<TextChunk> copy = this->textElements;
    std::sort(copy.begin(), copy.end(),
              [](TextChunk const &a, TextChunk const &b) {
                if (a.getTop() != b.getTop())
                  return a.getTop() < b.getTop();
                return a.getLeft() < b.getLeft();
              });
    std::string out;
    float curTop = copy.front().getTop();
    for (auto const &tc : copy) {
      if (useLineReturns && tc.getTop() > curTop) {
        out.push_back('\r');
      }
      out += tc.getText();
      curTop = tc.getTop();
    }
    // trim whitespace from both ends
    auto lpos = out.find_first_not_of(" \t\n\r");
    if (lpos == std::string::npos)
      return std::string();
    auto rpos = out.find_last_not_of(" \t\n\r");
    return out.substr(lpos, rpos - lpos + 1);
  }

  std::string getText() const { return getText(true); }

  bool isSpanning() const { return spanning; }
  void setSpanning(bool s) { spanning = s; }
  bool isPlaceholder() const { return placeholder; }
  void setPlaceholder(bool p) { placeholder = p; }

private:
  bool spanning;
  bool placeholder;
};

} // namespace tabula
