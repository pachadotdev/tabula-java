#pragma once
#include "tabula/HasText.h"
#include "tabula/Rectangle.h"
#include "tabula/TextElement.h"
#include <vector>

namespace tabula {

template <typename T>
class RectangularTextContainer : public Rectangle, public HasText {
public:
  RectangularTextContainer(float top, float left, float width, float height)
      : Rectangle(top, left, width, height) {}
  RectangularTextContainer() : Rectangle() {}

  RectangularTextContainer<T> &merge(const RectangularTextContainer<T> &other) {
    // append other's elements (no ordering semantics here)
    // Note: in Java this also merges geometry
    for (auto &e : other.textElements)
      textElements.push_back(e);
    Rectangle::merge(other);
    return *this;
  }

  const std::vector<T> &getTextElements() const { return textElements; }
  std::vector<T> &getTextElements() { return textElements; }
  void setTextElements(const std::vector<T> &v) { textElements = v; }

  // getText() implementations provided below

  // Generic implementation that assumes T has getText(), getTop(), getLeft()
  std::string getText(bool useLineReturns) const override {
    if (textElements.empty())
      return std::string();
    // make a local copy and sort like Java: by top then left
    auto copy = textElements;
    std::sort(copy.begin(), copy.end(), [](const T &a, const T &b) {
      if (a.getTop() != b.getTop())
        return a.getTop() < b.getTop();
      return a.getLeft() < b.getLeft();
    });
    std::string out;
    float curTop = copy.front().getTop();
    for (auto const &tc : copy) {
      if (useLineReturns && tc.getTop() > curTop)
        out.push_back('\r');
      out += tc.getText();
      curTop = tc.getTop();
    }
    // trim whitespace
    auto lpos = out.find_first_not_of(" \t\n\r");
    if (lpos == std::string::npos)
      return std::string();
    auto rpos = out.find_last_not_of(" \t\n\r");
    return out.substr(lpos, rpos - lpos + 1);
  }

  std::string getText() const override { return getText(true); }

  // convenience to merge a single text element's geometry into this container
  void merge(const T &elem) { Rectangle::merge(elem); }

  // helper to merge a container at index with another
  void mergeInPlace(const RectangularTextContainer<T> &other) {
    for (auto &e : other.textElements)
      textElements.push_back(e);
    Rectangle::merge(other);
  }

protected:
  std::vector<T> textElements;
};

} // namespace tabula
