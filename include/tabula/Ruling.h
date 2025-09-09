#pragma once
#include "Line.h"
#include "tabula/Rectangle.h"
#include "tabula/Utils.h"
#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

namespace tabula {

struct Ruling : public Line {
  Ruling(double x1_, double y1_, double x2_, double y2_)
      : Line(x1_, y1_, x2_, y2_) {
    normalize();
  }

  void normalize() {
    double angle = getAngle();
    if (Utils::within(angle, 0, 1) || Utils::within(angle, 180, 1)) {
      // almost horizontal: set both y to y1
      y2 = y1;
    } else if (Utils::within(angle, 90, 1) || Utils::within(angle, 270, 1)) {
      // almost vertical: set both x to x1
      x2 = x1;
    }
  }

  bool vertical() const { return length() > 0 && Utils::feq(x1, x2); }
  bool horizontal() const { return length() > 0 && Utils::feq(y1, y2); }
  bool oblique() const { return !(vertical() || horizontal()); }

  double getAngle() const {
    double angle = std::atan2(y2 - y1, x2 - x1) * 180.0 / M_PI;
    if (angle < 0)
      angle += 360;
    return angle;
  }

  double getLeft() const { return std::min(x1, x2); }
  double getRight() const { return std::max(x1, x2); }
  double getTop() const { return std::min(y1, y2); }
  double getBottom() const { return std::max(y1, y2); }

  double getPosition() const {
    if (oblique())
      throw std::runtime_error("oblique rulings don't have position");
    return vertical() ? getLeft() : getTop();
  }

  double getStart() const {
    if (oblique())
      throw std::runtime_error("oblique rulings don't have start");
    return vertical() ? getTop() : getLeft();
  }
  double getEnd() const {
    if (oblique())
      throw std::runtime_error("oblique rulings don't have end");
    return vertical() ? getBottom() : getRight();
  }

  void setPosition(double v) {
    if (oblique())
      throw std::runtime_error("oblique rulings don't have position");
    if (vertical()) {
      x1 = v;
      x2 = v;
    } else {
      y1 = v;
      y2 = v;
    }
  }
  void setStartEnd(double s, double e) {
    if (oblique())
      throw std::runtime_error("oblique rulings don't have start/end");
    if (vertical()) {
      y1 = s;
      y2 = e;
    } else {
      x1 = s;
      x2 = e;
    }
  }

  Ruling expand(double amount) const {
    Ruling r = *this;
    r.setStartEnd(getStart() - amount, getEnd() + amount);
    return r;
  }

  bool intersectsLine(const Ruling &other) const {
    // bounding box intersection test
    return !(getRight() < other.getLeft() || other.getRight() < getLeft() ||
             getBottom() < other.getTop() || other.getBottom() < getTop());
  }

  bool perpendicularTo(const Ruling &other) const {
    return vertical() == other.horizontal();
  }

  bool colinear(const std::pair<double, double> &point) const {
    double px = point.first, py = point.second;
    return px >= x1 && px <= x2 && py >= y1 && py <= y2;
  }

  bool nearlyIntersects(const Ruling &another) const {
    return nearlyIntersects(another, 1);
  }

  bool nearlyIntersects(const Ruling &another,
                        int colinearOrParallelExpandAmount) const {
    if (this->intersectsLine(another))
      return true;
    if (this->perpendicularTo(another)) {
      return this->expand(2.0).intersectsLine(another);
    } else {
      return this->expand(colinearOrParallelExpandAmount)
          .intersectsLine(another.expand(colinearOrParallelExpandAmount));
    }
  }

  std::pair<double, double> intersectionPoint(const Ruling &other) const {
    Ruling a = this->expand(2.0);
    Ruling b = other.expand(2.0);
    if (!a.intersectsLine(b))
      return {NAN, NAN};
    if (a.horizontal() && b.vertical())
      return {b.getLeft(), a.getTop()};
    if (a.vertical() && b.horizontal())
      return {a.getLeft(), b.getTop()};
    throw std::invalid_argument("lines must be orthogonal");
  }

  static std::vector<Ruling>
  cropRulingsToArea(const std::vector<Ruling> &rulings, const Rectangle &area) {
    std::vector<Ruling> rv;
    for (const auto &r : rulings) {
      if (!(r.getRight() < area.getLeft() || r.getLeft() > area.getRight() ||
            r.getBottom() < area.getTop() || r.getTop() > area.getBottom())) {
        double lx = std::max(r.getLeft(), static_cast<double>(area.getLeft()));
        double rx =
            std::min(r.getRight(), static_cast<double>(area.getRight()));
        double ty = std::max(r.getTop(), static_cast<double>(area.getTop()));
        double by =
            std::min(r.getBottom(), static_cast<double>(area.getBottom()));
        if (r.vertical())
          rv.emplace_back(lx, ty, lx, by);
        else
          rv.emplace_back(lx, ty, rx, ty);
      }
    }
    return rv;
  }

  static std::vector<Ruling> collapseOrientedRulings(std::vector<Ruling> lines,
                                                     int expandAmount = 1) {
    std::vector<Ruling> rv;
    std::sort(lines.begin(), lines.end(), [](const Ruling &a, const Ruling &b) {
      double diff = a.getPosition() - b.getPosition();
      if (Utils::feq(diff, 0.0)) {
        return a.getStart() < b.getStart();
      }
      return diff < 0.0;
    });

    for (auto &next_line : lines) {
      if (!rv.empty()) {
        Ruling &last = rv.back();
        if (Utils::feq(next_line.getPosition(), last.getPosition()) &&
            last.nearlyIntersects(next_line, expandAmount)) {
          float lastStart = static_cast<float>(last.getStart());
          float lastEnd = static_cast<float>(last.getEnd());
          bool lastFlipped = lastStart > lastEnd;
          bool nextFlipped = next_line.getStart() > next_line.getEnd();

          float nextS = nextFlipped ? static_cast<float>(next_line.getEnd())
                                    : static_cast<float>(next_line.getStart());
          float nextE = nextFlipped ? static_cast<float>(next_line.getStart())
                                    : static_cast<float>(next_line.getEnd());

          float newStart = lastFlipped ? std::max(nextS, lastStart)
                                       : std::min(nextS, lastStart);
          float newEnd =
              lastFlipped ? std::min(nextE, lastEnd) : std::max(nextE, lastEnd);
          last.setStartEnd(newStart, newEnd);
          continue;
        }
      }
      if (next_line.length() == 0)
        continue;
      rv.push_back(next_line);
    }
    return rv;
  }
};

} // namespace tabula
