#include "tabula/RectangularTextContainer.h"
#include "tabula/Table.h"
#include "tabula/TextChunk.h"
#include "tabula/Diagnostics.h"
#include <sstream>
#include <iomanip>
#include <iostream>

namespace tabula {
namespace json {

static std::string escape(const std::string &s) {
  std::ostringstream o;
  for (char c : s) {
    switch (c) {
    case '"':
      o << "\\\"";
      break;
    case '\\':
    {
      unsigned char uc = static_cast<unsigned char>(c);
      if (uc < 0x20) {
        // encode control characters as \u00XX
        std::ostringstream hexs;
        hexs << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << int(uc);
        o << "\\u00" << hexs.str();
      } else {
        o << c;
      }
    }
    break;
      break;
    case '\b':
      o << "\\b";
      break;
    case '\f':
      o << "\\f";
      break;
    case '\n':
      o << "\\n";
      break;
    case '\r':
      o << "\\r";
      break;
    case '\t':
      o << "\\t";
      break;
    default:
      o << c;
      break;
    }
  }
  return o.str();
}

// Note: we avoid declaring serializeRectangular for
// RectangularTextContainer<std::string> because the generic template
// implementation expects T to provide geometry methods.

std::string
serializeRectangular(const RectangularTextContainer<tabula::TextChunk> &rtc) {
  std::ostringstream o;
  o << "{";
  o << "\"top\":" << rtc.getTop() << ",";
  o << "\"left\":" << rtc.getLeft() << ",";
  o << "\"width\":" << rtc.getWidth() << ",";
  o << "\"height\":" << rtc.getHeight() << ",";
  o << "\"text\":\"" << escape(rtc.getText()) << "\"";
  o << "}";
  return o.str();
}

std::string serializeTable(const Table &table) {
  std::ostringstream o;
  // Diagnostic dump: print table grid and each cell text length/sample to stderr
  {
    auto const &rows = table.getRows();
    size_t rcount = rows.size();
    size_t ccount = (rcount ? rows.front().size() : 0);
    tabula::diag(std::string("[diag] serializeTable: page=") + std::to_string(table.getPageNumber()) + " rows=" + std::to_string(rcount) + " cols=" + std::to_string(ccount));
    for (size_t r = 0; r < rcount; ++r) {
      for (size_t c = 0; c < ccount; ++c) {
        auto const &cell = rows[r][c];
        std::string t = cell.getText();
        std::string sample = t.substr(0, std::min<size_t>(30, t.size()));
        // escape newlines in sample for compact logging
        for (char &ch : sample)
          if (ch == '\n' || ch == '\r')
            ch = ' ';
        tabula::diag(std::string("[diag] cell[") + std::to_string(r) + "," + std::to_string(c) + "] len=" + std::to_string(t.size()) + " sample=\"" + sample + "\"");
      }
    }
  }
  o << "{";
  o << "\"extraction_method\":\"\",";
  o << "\"page_number\":" << table.getPageNumber() << ",";
  o << "\"top\":" << table.getRect().getTop() << ",";
  o << "\"left\":" << table.getRect().getLeft() << ",";
  o << "\"width\":" << table.getRect().getWidth() << ",";
  o << "\"height\":" << table.getRect().getHeight() << ",";
  o << "\"right\":" << table.getRect().getRight() << ",";
  o << "\"bottom\":" << table.getRect().getBottom() << ",";
  o << "\"data\": [";
  bool firstRow = true;
  for (auto const &row : table.getRows()) {
    if (!firstRow)
      o << ",";
    firstRow = false;
    o << "[";
    bool firstCell = true;
    for (auto const &cell : row) {
      if (!firstCell)
        o << ",";
      firstCell = false;
      // serialize the cell geometry and text directly
      o << "{";
      o << "\"top\":" << cell.getTop() << ",";
      o << "\"left\":" << cell.getLeft() << ",";
      o << "\"width\":" << cell.getWidth() << ",";
      o << "\"height\":" << cell.getHeight() << ",";
      o << "\"text\":\"" << escape(cell.getText()) << "\"";
      o << "}";
    }
    o << "]";
  }
  o << "]";
  o << "}";
  return o.str();
}

} // namespace json
} // namespace tabula
