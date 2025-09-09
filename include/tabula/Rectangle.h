#pragma once
#include <algorithm>
#include <array>
#include <limits>
#include <ostream>
#include <utility>
#include <vector>

namespace tabula {

struct Rectangle {
  float top;
  float left;
  float width;
  float height;

  static constexpr float VERTICAL_COMPARISON_THRESHOLD = 0.4f;

  Rectangle() : top(0), left(0), width(0), height(0) {}
  Rectangle(float t, float l, float w, float h)
      : top(t), left(l), width(w), height(h) {}

  float getTop() const { return top; }
  void setTop(float t) {
    float deltaHeight = t - top;
    top = t;
    height = height - deltaHeight;
  }

  float getLeft() const { return left; }
  void setLeft(float l) {
    float deltaWidth = l - left;
    left = l;
    width = width - deltaWidth;
  }

  float getRight() const { return left + width; }
  void setRight(float r) { width = r - left; }

  float getBottom() const { return top + height; }
  void setBottom(float b) { height = b - top; }

  float getArea() const { return width * height; }

  float getWidth() const { return width; }
  float getHeight() const { return height; }

  bool contains(Rectangle const &other) const {
    return other.getLeft() >= getLeft() && other.getRight() <= getRight() &&
           other.getTop() >= getTop() && other.getBottom() <= getBottom();
  }

  float verticalOverlap(Rectangle const &other) const {
    return std::max(0.0f, std::min(getBottom(), other.getBottom()) -
                              std::max(getTop(), other.getTop()));
  }

  bool verticallyOverlaps(Rectangle const &other) const {
    return verticalOverlap(other) > 0.0f;
  }

  float horizontalOverlap(Rectangle const &other) const {
    return std::max(0.0f, std::min(getRight(), other.getRight()) -
                              std::max(getLeft(), other.getLeft()));
  }

  bool horizontallyOverlaps(Rectangle const &other) const {
    return horizontalOverlap(other) > 0.0f;
  }

  float verticalOverlapRatio(Rectangle const &other) const {
    float rv = 0.0f;
    float delta =
        std::min(getBottom() - getTop(), other.getBottom() - other.getTop());
    if (delta <= 0)
      return 0.0f;

    if (other.getTop() <= getTop() && getTop() <= other.getBottom() &&
        other.getBottom() <= getBottom()) {
      rv = (other.getBottom() - getTop()) / delta;
    } else if (getTop() <= other.getTop() && other.getTop() <= getBottom() &&
               getBottom() <= other.getBottom()) {
      rv = (getBottom() - other.getTop()) / delta;
    } else if (getTop() <= other.getTop() &&
               other.getTop() <= other.getBottom() &&
               other.getBottom() <= getBottom()) {
      rv = (other.getBottom() - other.getTop()) / delta;
    } else if (other.getTop() <= getTop() && getTop() <= getBottom() &&
               getBottom() <= other.getBottom()) {
      rv = (getBottom() - getTop()) / delta;
    }
    return rv;
  }

  float overlapRatio(Rectangle const &other) const {
    double intersectionWidth =
        std::max(0.0, std::min((double)getRight(), (double)other.getRight()) -
                          std::max((double)getLeft(), (double)other.getLeft()));
    double intersectionHeight =
        std::max(0.0, std::min((double)getBottom(), (double)other.getBottom()) -
                          std::max((double)getTop(), (double)other.getTop()));
    double intersectionArea =
        std::max(0.0, intersectionWidth * intersectionHeight);
    double unionArea =
        (double)getArea() + (double)other.getArea() - intersectionArea;
    if (unionArea <= 0.0)
      return 0.0f;
    return static_cast<float>(intersectionArea / unionArea);
  }

  Rectangle &merge(Rectangle const &other) {
    float minx = std::min(getLeft(), other.getLeft());
    float miny = std::min(getTop(), other.getTop());
    float maxx = std::max(getRight(), other.getRight());
    float maxy = std::max(getBottom(), other.getBottom());
    left = minx;
    top = miny;
    width = maxx - minx;
    height = maxy - miny;
    return *this;
  }

  std::array<std::pair<float, float>, 4> getPoints() const {
    return {std::make_pair(getLeft(), getTop()),
            std::make_pair(getRight(), getTop()),
            std::make_pair(getRight(), getBottom()),
            std::make_pair(getLeft(), getBottom())};
  }

  int isLtrDominant() const { return 0; }

  static Rectangle boundingBoxOf(const std::vector<Rectangle> &rectangles) {
    float minx = std::numeric_limits<float>::max();
    float miny = std::numeric_limits<float>::max();
    float maxx = std::numeric_limits<float>::lowest();
    float maxy = std::numeric_limits<float>::lowest();

    for (auto const &r : rectangles) {
      minx = std::min(r.getLeft(), minx);
      miny = std::min(r.getTop(), miny);
      maxx = std::max(r.getRight(), maxx);
      maxy = std::max(r.getBottom(), maxy);
    }
    if (rectangles.empty())
      return Rectangle();
    return Rectangle(miny, minx, maxx - minx, maxy - miny);
  }
};

inline std::ostream &operator<<(std::ostream &os, Rectangle const &r) {
  os << "Rectangle(top=" << r.getTop() << ", left=" << r.getLeft()
     << ", w=" << r.width << ", h=" << r.height << ", bottom=" << r.getBottom()
     << ", right=" << r.getRight() << ")";
  return os;
}

} // namespace tabula
