#pragma once
#include "tabula/Cell.h"
#include "tabula/Rectangle.h"
#include "tabula/RectangleSpatialIndex.h"
#include "tabula/Ruling.h"
#include "tabula/Table.h"
#include <algorithm>
#include <vector>

namespace tabula {

// Port of Java TableWithRulingLines: uses RectangleSpatialIndex to compute
// start columns
class TableWithRulingLines : public Table {
public:
  TableWithRulingLines(const Rectangle &area, const std::vector<Cell> &cells,
                       const std::vector<Ruling> &horizontalRulings,
                       const std::vector<Ruling> &verticalRulings,
                       int pageNumber) {
    // store metadata similar to Java constructor
    this->setRect(area);
    this->setPageNumber(pageNumber);
    this->horizontalRulings = horizontalRulings;
    this->verticalRulings = verticalRulings;
    if (cells.empty())
      return;

    RectangleSpatialIndex<Cell> si; // default cell size
    for (auto const &c : cells)
      si.add(c);

    // compute rows of cells: group by top coordinate
    auto rowsOfCells = [](const std::vector<Cell> &cellsInput) {
      std::vector<Cell> copy = cellsInput;
      std::sort(copy.begin(), copy.end(), [](Cell const &a, Cell const &b) {
        if (a.getTop() != b.getTop())
          return a.getTop() < b.getTop();
        return a.getLeft() < b.getLeft();
      });
      std::vector<std::vector<Cell>> rows;
      if (!copy.empty()) {
        float lastTop = copy.front().getTop();
        rows.push_back(std::vector<Cell>{copy.front()});
        for (size_t i = 1; i < copy.size(); ++i) {
          const Cell &c = copy[i];
          if (!feq(c.getTop(), lastTop)) {
            rows.emplace_back();
          }
          rows.back().push_back(c);
          lastTop = c.getTop();
        }
      }
      return rows;
    };

    std::vector<std::vector<Cell>> rows = rowsOfCells(cells);

    // If no vertical rulings are present, fall back to Java's addCells
    // algorithm which assigns columns using spatial-index containment. This
    // preserves behavior for PDFs without ruling lines.
    if (verticalRulings.empty() && horizontalRulings.empty()) {
      for (size_t i = 0; i < rows.size(); ++i) {
        auto &row = rows[i];
        if (row.empty())
          continue;
        // first cell in the row
        const Cell &cell = row.front();
        // build a query rectangle below and to the left of the cell as in Java
        // implementation
        Rectangle bounds = si.getBounds();
        Rectangle q(cell.getBottom(), bounds.getLeft(),
                    cell.getLeft() - bounds.getLeft(),
                    bounds.getBottom() - cell.getBottom());
        std::vector<Cell> othersCandidates = si.contains(q);
        auto otherRows = rowsOfCells(othersCandidates);
        int startColumn = 0;
        for (auto &r : otherRows)
          startColumn = std::max(startColumn, static_cast<int>(r.size()));
        // place first cell
        this->add(cell, static_cast<int>(i), startColumn++);
        // place remaining cells in the row
        for (size_t k = 1; k < row.size(); ++k)
          this->add(row[k], static_cast<int>(i), startColumn++);
      }
      return;
    }

    // Determine column boundaries using vertical rulings and area bounds
    std::vector<double> xs;
    xs.push_back(static_cast<double>(area.getLeft()));
    for (auto const &vr : verticalRulings) {
      if (vr.vertical())
        xs.push_back(vr.getLeft());
    }
    xs.push_back(static_cast<double>(area.getRight()));
    std::sort(xs.begin(), xs.end());
    xs.erase(std::unique(xs.begin(), xs.end()), xs.end());

    // Determine row boundaries using horizontal rulings and area bounds
    std::vector<double> ys;
    ys.push_back(static_cast<double>(area.getTop()));
    for (auto const &hr : horizontalRulings) {
      if (hr.horizontal())
        ys.push_back(hr.getTop());
    }
    ys.push_back(static_cast<double>(area.getBottom()));
    std::sort(ys.begin(), ys.end());
    ys.erase(std::unique(ys.begin(), ys.end()), ys.end());

    // Iterate over each cell and compute its grid position and span
    // (rowspan/colspan)
    for (auto const &c0 : cells) {
      Cell c = c0;
      double cleft = c.getLeft();
      double cright = c.getRight();
      double ctop = c.getTop();
      double cbottom = c.getBottom();

      auto itL = std::upper_bound(xs.begin(), xs.end(), cleft);
      int startCol =
          std::max(0, static_cast<int>(std::distance(xs.begin(), itL)) - 1);
      auto itR = std::lower_bound(xs.begin(), xs.end(), cright);
      int endCol =
          std::max(0, static_cast<int>(std::distance(xs.begin(), itR)) - 1);
      int colspan = std::max(1, endCol - startCol + 1);

      auto itT = std::upper_bound(ys.begin(), ys.end(), ctop);
      int startRow =
          std::max(0, static_cast<int>(std::distance(ys.begin(), itT)) - 1);
      auto itB = std::lower_bound(ys.begin(), ys.end(), cbottom);
      int endRow =
          std::max(0, static_cast<int>(std::distance(ys.begin(), itB)) - 1);
      int rowspan = std::max(1, endRow - startRow + 1);

      if (colspan > 1 || rowspan > 1)
        c.setSpanning(true);

      // Place the main cell at top-left grid (startRow,startCol)
      this->add(c, startRow, startCol);

      // Place placeholders for colspan in the same row
      for (int kc = 1; kc < colspan; ++kc) {
        double lx = xs[startCol + kc];
        double rx = (startCol + kc + 1 < static_cast<int>(xs.size()))
                        ? xs[startCol + kc + 1]
                        : xs.back();
        Cell ph(c.getTop(), static_cast<float>(lx), static_cast<float>(rx - lx),
                c.getHeight());
        ph.setPlaceholder(true);
        this->add(ph, startRow, startCol + kc);
      }

      // Place placeholders for rowspan in following rows (including colspan
      // placeholders for those rows)
      for (int kr = 1; kr < rowspan; ++kr) {
        int rrow = startRow + kr;
        // main placeholder column block
        for (int kc = 0; kc < colspan; ++kc) {
          double lx = xs[startCol + kc];
          double rx = (startCol + kc + 1 < static_cast<int>(xs.size()))
                          ? xs[startCol + kc + 1]
                          : xs.back();
          Cell ph(c.getTop(), static_cast<float>(lx),
                  static_cast<float>(rx - lx), c.getHeight());
          ph.setPlaceholder(true);
          this->add(ph, rrow, startCol + kc);
        }
      }
    }
  }

private:
private:
  std::vector<Ruling> verticalRulingLines;
  std::vector<Ruling> horizontalRulingLines;
  std::vector<Ruling> verticalRulings;
  std::vector<Ruling> horizontalRulings;

  static bool feq(float a, float b) { return std::abs(a - b) < 1e-6f; }
};

} // namespace tabula
