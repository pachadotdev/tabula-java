#pragma once
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

namespace tabula {

struct Utils {
  static bool within(double first, double second, double variance) {
    return second < first + variance && second > first - variance;
  }
  static bool overlap(double y1, double height1, double y2, double height2,
                      double variance) {
    return within(y1, y2, variance) || (y2 <= y1 && y2 >= y1 - height1) ||
           (y1 <= y2 && y1 >= y2 - height2);
  }
  static bool overlap(double y1, double height1, double y2, double height2) {
    return overlap(y1, height1, y2, height2, 0.1f);
  }
  static bool feq(double f1, double f2) {
    constexpr double EPSILON = 0.01;
    return std::abs(f1 - f2) < EPSILON;
  }
  static float round(double d, int decimalPlace) {
    double scale = std::pow(10.0, decimalPlace);
    return static_cast<float>(std::round(d * scale) / scale);
  }
  static bool isNumeric(const std::string &s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
  }
  static std::string join(const std::string &glue,
                          const std::vector<std::string> &s) {
    if (s.empty())
      return "";
    std::string out = s[0];
    for (size_t i = 1; i < s.size(); ++i)
      out += glue + s[i];
    return out;
  }
  template <typename T>
  static std::vector<std::vector<T>>
  transpose(const std::vector<std::vector<T>> &table) {
    if (table.empty())
      return {};
    size_t N = table[0].size();
    std::vector<std::vector<T>> ret(N);
    for (size_t i = 0; i < N; ++i) {
      for (const auto &row : table)
        ret[i].push_back(row[i]);
    }
    return ret;
  }
  // return vector of ints [begin, end) similar to Java range(begin,end)
  static std::vector<int> range(int begin, int end) {
    std::vector<int> out;
    if (end <= begin)
      return out;
    out.reserve(end - begin);
    for (int i = begin; i < end; ++i)
      out.push_back(i);
    return out;
  }

  // bounds of a collection of Rectangles (throws if empty)
  template <typename RectT>
  static RectT bounds(const std::vector<RectT> &rects) {
    if (rects.empty())
      throw std::invalid_argument("shapes can't be empty");
    RectT rv = rects[0];
    for (size_t i = 1; i < rects.size(); ++i)
      rv = rv.merge(rects[i]), rv;
    return rv;
  }

  // parse pages option: returns vector<int> of pages, or vector{-1} to mean
  // "all"
  static std::vector<int> parsePagesOption(const std::string &pagesSpec) {
    if (pagesSpec == "all")
      return std::vector<int>{-1};
    std::vector<int> rv;
    size_t start = 0;
    while (start < pagesSpec.size()) {
      auto comma = pagesSpec.find(',', start);
      std::string token = pagesSpec.substr(start, (comma == std::string::npos)
                                                      ? std::string::npos
                                                      : comma - start);
      auto dash = token.find('-');
      if (dash == std::string::npos) {
        if (!isNumeric(token))
          throw std::invalid_argument(
              "Syntax error in page range specification");
        rv.push_back(std::stoi(token));
      } else {
        std::string a = token.substr(0, dash);
        std::string b = token.substr(dash + 1);
        if (!isNumeric(a) || !isNumeric(b))
          throw std::invalid_argument(
              "Syntax error in page range specification");
        int t = std::stoi(a), f = std::stoi(b);
        if (t > f)
          throw std::invalid_argument(
              "Syntax error in page range specification");
        auto r = range(t, f + 1);
        rv.insert(rv.end(), r.begin(), r.end());
      }
      if (comma == std::string::npos)
        break;
      start = comma + 1;
    }
    std::sort(rv.begin(), rv.end());
    return rv;
  }

  // Snap endpoints of lines: group points within xThreshold/yThreshold and set
  // them to group average
  template <typename LineT>
  static void snapPoints(std::vector<LineT> &rulings, float xThreshold,
                         float yThreshold) {
    using PointT = std::pair<float, float>;
    // collect points and map lines to their two points
    struct LP {
      LineT *line;
      PointT p1;
      PointT p2;
    };
    std::vector<LP> lps;
    lps.reserve(rulings.size());
    std::vector<PointT *> allPoints;

    // We'll keep an array of points objects so we can mutate them
    std::vector<PointT> points;
    for (auto &r : rulings) {
      PointT p1{static_cast<float>(r.x1), static_cast<float>(r.y1)};
      PointT p2{static_cast<float>(r.x2), static_cast<float>(r.y2)};
      points.push_back(p1);
      points.push_back(p2);
      lps.push_back(
          LP{&r, points[points.size() - 2], points[points.size() - 1]});
    }

    if (points.empty())
      return;

    // helper: group by axis
    auto group_and_snap = [&](bool byX) {
      // create indices sorted by axis
      std::vector<int> idx(points.size());
      for (size_t i = 0; i < points.size(); ++i)
        idx[i] = (int)i;
      std::sort(idx.begin(), idx.end(), [&](int a, int b) {
        return (byX ? points[a].first : points[a].second) <
               (byX ? points[b].first : points[b].second);
      });

      std::vector<std::vector<int>> groups;
      groups.emplace_back();
      groups.back().push_back(idx[0]);
      for (size_t k = 1; k < idx.size(); ++k) {
        int cur = idx[k];
        int last = groups.back()[0];
        float curVal = byX ? points[cur].first : points[cur].second;
        float lastVal = byX ? points[last].first : points[last].second;
        if (std::fabs(curVal - lastVal) < (byX ? xThreshold : yThreshold)) {
          groups.back().push_back(cur);
        } else {
          groups.emplace_back();
          groups.back().push_back(cur);
        }
      }

      for (auto &g : groups) {
        float avg = 0.0f;
        for (int ii : g)
          avg += (byX ? points[ii].first : points[ii].second);
        avg /= (float)g.size();
        for (int ii : g) {
          if (byX)
            points[ii].first = avg;
          else
            points[ii].second = avg;
        }
      }
    };

    // snap X then Y
    group_and_snap(true);
    group_and_snap(false);

    // write back into rulings by updating endpoint coordinates
    size_t pi = 0;
    for (auto &r : rulings) {
      // each ruling had two points in sequence
      r.x1 = points[pi].first;
      r.y1 = points[pi].second;
      r.x2 = points[pi + 1].first;
      r.y2 = points[pi + 1].second;
      // also update Rectangle-derived geometry
      float newLeft = std::min(points[pi].first, points[pi + 1].first);
      float newTop = std::min(points[pi].second, points[pi + 1].second);
      float newRight = std::max(points[pi].first, points[pi + 1].first);
      float newBottom = std::max(points[pi].second, points[pi + 1].second);
      r.left = newLeft;
      r.top = newTop;
      r.width = newRight - newLeft;
      r.height = newBottom - newTop;
      pi += 2;
    }
  }
};

} // namespace tabula
