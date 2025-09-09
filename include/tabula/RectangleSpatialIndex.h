#pragma once
#include "tabula/Rectangle.h"
#include <cmath>
#include <limits>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace tabula {

// Template spatial index for rectangle-like objects (T must expose
// Rectangle-like getters)
template <typename T> class RectangleSpatialIndex {
public:
  explicit RectangleSpatialIndex(float cellSize_ = 50.0f)
      : cellSize(cellSize_) {}

  void add(const T &obj) {
    size_t id = rects.size();
    rects.push_back(obj);
    int minX, minY, maxX, maxY;
    std::tie(minX, minY, maxX, maxY) = cellRangeForObj(obj);
    for (int ix = minX; ix <= maxX; ++ix) {
      for (int iy = minY; iy <= maxY; ++iy) {
        grid[cellKey(ix, iy)].push_back(id);
      }
    }
  }

  // alias for compatibility
  void insert(const T &obj) { add(obj); }

  // returns objects that overlap the query area
  std::vector<T> query(Rectangle const &area) const {
    std::vector<T> out;
    if (rects.empty())
      return out;
    int minX, minY, maxX, maxY;
    std::tie(minX, minY, maxX, maxY) = cellRangeFor(area);
    std::unordered_set<size_t> seen;
    for (int ix = minX; ix <= maxX; ++ix) {
      for (int iy = minY; iy <= maxY; ++iy) {
        auto it = grid.find(cellKey(ix, iy));
        if (it == grid.end())
          continue;
        for (size_t id : it->second) {
          if (seen.insert(id).second) {
            const T &r = rects[id];
            if (!(r.getRight() < area.getLeft() ||
                  r.getLeft() > area.getRight() ||
                  r.getBottom() < area.getTop() ||
                  r.getTop() > area.getBottom())) {
              out.push_back(r);
            }
          }
        }
      }
    }
    return out;
  }

  // returns objects fully contained inside 'area'
  std::vector<T> contains(Rectangle const &area) const {
    std::vector<T> candidates = query(area);
    std::vector<T> out;
    for (auto const &c : candidates) {
      if (area.contains(c))
        out.push_back(c);
    }
    return out;
  }

  std::vector<T> intersects(Rectangle const &area) const { return query(area); }

  Rectangle getBounds() const {
    if (rects.empty())
      return Rectangle();
    std::vector<Rectangle> tmp;
    tmp.reserve(rects.size());
    for (auto const &r : rects)
      tmp.emplace_back(r.getTop(), r.getLeft(), r.getWidth(), r.getHeight());
    return Rectangle::boundingBoxOf(tmp);
  }

  void clear() {
    rects.clear();
    grid.clear();
  }

private:
  float cellSize;
  std::vector<T> rects;
  std::unordered_map<long long, std::vector<size_t>> grid;

  static inline long long cellKey(int x, int y) {
    return (static_cast<long long>(x) << 32) ^ static_cast<unsigned int>(y);
  }

  std::tuple<int, int, int, int> cellRangeFor(const Rectangle &r) const {
    int minX = static_cast<int>(std::floor(r.getLeft() / cellSize));
    int maxX = static_cast<int>(std::floor(r.getRight() / cellSize));
    int minY = static_cast<int>(std::floor(r.getTop() / cellSize));
    int maxY = static_cast<int>(std::floor(r.getBottom() / cellSize));
    return {minX, minY, maxX, maxY};
  }

  // overload to accept T
  std::tuple<int, int, int, int> cellRangeForObj(const T &t) const {
    return cellRangeFor(
        Rectangle(t.getTop(), t.getLeft(), t.getWidth(), t.getHeight()));
  }
};

} // namespace tabula
