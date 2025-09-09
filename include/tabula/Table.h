#pragma once
#include "tabula/Cell.h"
#include <algorithm>
#include <map>
#include <utility>
#include <vector>

namespace tabula {

class Table {
public:
  Table() = default;

  // page number metadata
  void setPageNumber(int pn) { pageNumber = pn; }
  int getPageNumber() const { return pageNumber; }

  // set/get rectangle bounds of the table
  void setRect(const Rectangle &r) { rect = r; }
  Rectangle getRect() const { return rect; }

  // Optional column headers used by CSVWriter when emitting header rows
  const std::vector<std::string> &getColumnHeaders() const {
    return columnHeaders;
  }
  void setColumnHeaders(const std::vector<std::string> &headers) {
    columnHeaders = headers;
  }

  // Add a prebuilt row (keeps backward compatibility)
  void addRow(const std::vector<Cell> &row) {
    // append row to internal grid at next index
    int rowIndex = rowCount;
    for (int c = 0; c < static_cast<int>(row.size()); ++c) {
      add(row[c], rowIndex, c);
    }
  }

  // Add a single cell at (row,col). Merges if a cell already exists at that
  // position.
  void add(const Cell &cell, int row, int col) {
    if (row < 0 || col < 0)
      return;
    rowCount = std::max(rowCount, row + 1);
    colCount = std::max(colCount, col + 1);
    std::pair<int, int> key(row, col);
    auto it = cells.find(key);
    if (it != cells.end()) {
      // merge geometries and text elements into existing cell
      it->second.mergeInPlace(cell);
    } else {
      cells.emplace(key, cell);
    }
    memoizedRows.clear();
  }

  int getRowCount() const { return rowCount; }
  int getColCount() const { return colCount; }

  const std::vector<std::vector<Cell>> &getRows() const {
    if (memoizedRows.empty())
      computeRows();
    return memoizedRows;
  }

private:
  void computeRows() const {
    memoizedRows.clear();
    memoizedRows.resize(rowCount);
    for (int r = 0; r < rowCount; ++r) {
      memoizedRows[r].resize(colCount);
    }
    for (auto const &p : cells) {
      int r = p.first.first;
      int c = p.first.second;
      if (r < rowCount && c < colCount)
        memoizedRows[r][c] = p.second;
    }
  }

  mutable std::vector<std::vector<Cell>> memoizedRows;
  std::map<std::pair<int, int>, Cell> cells;
  int rowCount = 0;
  int colCount = 0;
  int pageNumber = 0;
  Rectangle rect;
  std::vector<std::string> columnHeaders;
};

} // namespace tabula
