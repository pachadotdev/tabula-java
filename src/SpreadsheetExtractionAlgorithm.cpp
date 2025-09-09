#include "tabula/SpreadsheetExtractionAlgorithm.h"
#include "tabula/Ruling.h"
#include "tabula/TableWithRulingLines.h"
#include "tabula/TextElement.h"
#include "tabula/Utils.h"
#include "tabula/TextChunk.h"
#include "tabula/Diagnostics.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <utility>

namespace tabula {

std::vector<Table>
SpreadsheetExtractionAlgorithm::extract(const Page &page) const {
  std::vector<Ruling> horizontals;
  std::vector<Ruling> verticals;
  for (auto const &r : page.getRulings()) {
    if (r.horizontal())
      horizontals.push_back(r);
    else if (r.vertical())
      verticals.push_back(r);
  }
  horizontals = Ruling::collapseOrientedRulings(horizontals);
  verticals = Ruling::collapseOrientedRulings(verticals);

  auto cells = findCells(horizontals, verticals);
  // Diagnostic: print number of detected cells and sample coordinates
  try {
    tabula::diag(std::string("[diag] detected cells=") + std::to_string(cells.size()));
    size_t maxc = 20;
    for (size_t ci = 0; ci < cells.size() && ci < maxc; ++ci) {
      const auto &cc = cells[ci];
      tabula::diag(std::string("[diag] CELL idx=") + std::to_string(ci) + " top=" + std::to_string(cc.getTop()) + " left=" + std::to_string(cc.getLeft()) + " w=" + std::to_string(cc.getWidth()) + " h=" + std::to_string(cc.getHeight()));
    }
  } catch (...) {}
  // compute collapsed vertical rulings for per-cell merge
  std::vector<Ruling> vertRulings = Ruling::collapseOrientedRulings(verticals);
  auto areas = findSpreadsheetsFromCells(
      std::vector<Rectangle>(cells.begin(), cells.end()));

  std::vector<Table> tables;
  for (auto const &area : areas) {
    std::vector<Cell> overlapping;
    for (auto &c : cells) {
      if (c.horizontalOverlap(area) > 0.0f || c.verticalOverlap(area) > 0.0f ||
          c.overlapRatio(area) > 0.0f) {
        // Collect raw TextElements inside this cell and merge them into
        // word-level TextChunks (mirror Java's per-cell merge)
        std::vector<TextElement> elems;
        for (auto const &tc : page.getTextChunks()) {
          for (auto const &te : tc.getTextElements()) {
            if (te.overlapRatio(c) > 0.0f || c.overlapRatio(te) > 0.0f)
              elems.push_back(te);
          }
        }
        if (!elems.empty()) {
          // diagnostic: report raw element count and a sample
          try {
            std::string samp = elems.front().getText();
            if (samp.size() > 80)
              samp = samp.substr(0, 80) + "...";
            tabula::diag(std::string("[diag] cell_raw_elems top=") + std::to_string(c.getTop()) + " left=" + std::to_string(c.getLeft()) + " count=" + std::to_string(elems.size()) + " sample=\"" + samp + "\"");
          } catch (...) {}
          auto merged = TextChunk::mergeWords(elems, &vertRulings);
          std::vector<TextChunk> tcs = merged;
          c.setTextElements(tcs);
        } else {
          tabula::diag(std::string("[diag] cell_raw_elems top=") + std::to_string(c.getTop()) + " left=" + std::to_string(c.getLeft()) + " count=0");
        }
        overlapping.push_back(c);
      }
    }

    // crop rulings that intersect this area so the table constructor uses
    // only local segments
    std::vector<Ruling> horizCrop =
        Ruling::cropRulingsToArea(horizontals, area);
    std::vector<Ruling> vertCrop = Ruling::cropRulingsToArea(verticals, area);

    // page number not currently stored on Page; pass 0 as placeholder
    TableWithRulingLines tw(area, overlapping, horizCrop, vertCrop,
                            page.getPageNumber());
    // slice into Table vector (TableWithRulingLines populates base Table
    // fields)
    tables.push_back(tw);
  }
  return tables;
}

std::vector<Cell>
SpreadsheetExtractionAlgorithm::findCells(const std::vector<Ruling> &horizontal,
                                          const std::vector<Ruling> &vertical) {
  std::vector<Cell> cellsFound;
  // Build intersection map similar to Java implementation
  std::map<std::pair<double, double>, std::pair<Ruling, Ruling>> intersections;
  for (std::vector<Ruling>::const_iterator ih = horizontal.begin();
       ih != horizontal.end(); ++ih) {
    const Ruling &h = *ih;
    for (std::vector<Ruling>::const_iterator iv = vertical.begin();
         iv != vertical.end(); ++iv) {
      const Ruling &v = *iv;
      auto p = h.intersectionPoint(v);
      if (!std::isnan(p.first) && !std::isnan(p.second)) {
        intersections.emplace(std::make_pair(p.first, p.second),
                              std::make_pair(h.expand(2.0), v.expand(2.0)));
      }
    }
  }

  // Convert keys to vector and sort Y-first
  std::vector<std::pair<double, double>> points;
  for (std::map<std::pair<double, double>,
                std::pair<Ruling, Ruling>>::const_iterator it =
           intersections.begin();
       it != intersections.end(); ++it)
    points.push_back(it->first);
  std::sort(points.begin(), points.end(),
            [](const std::pair<double, double> &a,
               const std::pair<double, double> &b) {
              if (a.second != b.second)
                return a.second < b.second;
              return a.first < b.first;
            });

  for (size_t i = 0; i < points.size(); ++i) {
    auto topLeft = points[i];
    std::map<std::pair<double, double>,
             std::pair<Ruling, Ruling>>::const_iterator hv_it =
        intersections.find(topLeft);
    if (hv_it == intersections.end())
      continue;
    const std::pair<Ruling, Ruling> &hv = hv_it->second;

    std::vector<std::pair<double, double>> xPoints, yPoints;
    for (size_t j = i; j < points.size(); ++j) {
      auto p = points[j];
      if (p.first == topLeft.first && p.second > topLeft.second)
        xPoints.push_back(p);
      if (p.second == topLeft.second && p.first > topLeft.first)
        yPoints.push_back(p);
    }

    bool outer_break = false;
    for (auto const &xp : xPoints) {
      // vertical edge check - compare vertical position
      std::map<std::pair<double, double>,
               std::pair<Ruling, Ruling>>::const_iterator xp_it =
          intersections.find(xp);
      if (xp_it == intersections.end())
        continue;
      if (!Utils::feq(xp_it->second.second.getLeft(), hv.second.getLeft()))
        continue;
      for (auto const &yp : yPoints) {
        std::map<std::pair<double, double>,
                 std::pair<Ruling, Ruling>>::const_iterator yp_it =
            intersections.find(yp);
        if (yp_it == intersections.end())
          continue;
        if (!Utils::feq(yp_it->second.first.getTop(), hv.first.getTop()))
          continue;
        std::pair<double, double> btmRight = {yp.first, xp.second};
        std::map<std::pair<double, double>,
                 std::pair<Ruling, Ruling>>::const_iterator br_it =
            intersections.find(btmRight);
        if (br_it != intersections.end()) {
          const std::pair<Ruling, Ruling> &br = br_it->second;
          std::map<std::pair<double, double>,
                   std::pair<Ruling, Ruling>>::const_iterator xp_it2 =
              intersections.find(xp);
          std::map<std::pair<double, double>,
                   std::pair<Ruling, Ruling>>::const_iterator yp_it2 =
              intersections.find(yp);
          if (xp_it2 == intersections.end() || yp_it2 == intersections.end())
            continue;
          if (Utils::feq(br.first.getTop(), xp_it2->second.first.getTop()) &&
              Utils::feq(br.second.getLeft(),
                         yp_it2->second.second.getLeft())) {
            double top = topLeft.second;
            double left = topLeft.first;
            double right = btmRight.first;
            double bottom = btmRight.second;
            cellsFound.emplace_back(static_cast<float>(top),
                                    static_cast<float>(left),
                                    static_cast<float>(right - left),
                                    static_cast<float>(bottom - top));
            outer_break = true;
            break;
          }
        }
      }
      if (outer_break)
        break;
    }
  }

  return cellsFound;
}

std::vector<Rectangle>
SpreadsheetExtractionAlgorithm::findSpreadsheetsFromCells(
    const std::vector<Rectangle> &cells) {
  // Reuse Java approach: symmetric difference of vertices to form polygons
  std::vector<Rectangle> rectangles;
  std::set<std::pair<double, double>> pointSet;
  for (auto const &cell : cells) {
    for (auto const &pt : cell.getPoints())
      pointSet.insert({pt.first, pt.second});
  }
  // Simplified: return bounding box of all points as single rectangle if any
  if (pointSet.empty())
    return rectangles;
  double top = 1e9, left = 1e9, bottom = -1e9, right = -1e9;
  for (auto const &p : pointSet) {
    left = std::min(left, p.first);
    right = std::max(right, p.first);
    top = std::min(top, p.second);
    bottom = std::max(bottom, p.second);
  }
  rectangles.emplace_back(static_cast<float>(top), static_cast<float>(left),
                          static_cast<float>(right - left),
                          static_cast<float>(bottom - top));
  return rectangles;
}

} // namespace tabula
