#include "tabula/writers/CSVWriter.h"
#include <iomanip>

namespace tabula {
namespace writers {

static std::string escapeCSV(const std::string &s, char delim) {
  bool needQuotes = false;
  for (char c : s) {
    if (c == '"' || c == '\n' || c == '\r' || c == delim) {
      needQuotes = true;
      break;
    }
  }
  if (!needQuotes)
    return s;
  std::string out;
  out.push_back('"');
  for (char c : s) {
    if (c == '"')
      out.push_back('"');
    out.push_back(c);
  }
  out.push_back('"');
  return out;
}

void CSVWriter::write(std::ostream &out, const Table &table) const {
  write(out, std::vector<Table>{table});
}

void CSVWriter::write(std::ostream &out,
                      const std::vector<Table> &tables) const {
  bool firstTable = true;
  for (auto const &t : tables) {
    if (!firstTable) {
      // separate multiple tables by an empty line
      out << "\n";
    }
    firstTable = false;
    // optional header row
    if (includeHeaderRow) {
      bool first = true;
      if (includePageNumber) {
        out << "page";
        first = false;
      }
      for (auto const &colHeader : t.getColumnHeaders()) {
        if (!first) {
          out << delim;
        }
        first = false;
        out << escapeCSV(colHeader, delim);
      }
      out << "\n";
    }
    for (auto const &row : t.getRows()) {
      bool first = true;
      // optionally write page number as first column
      if (includePageNumber) {
        out << t.getPageNumber();
        if (!row.empty())
          out << delim;
      }
      for (auto const &cell : row) {
        if (!first) {
          out << delim;
        }
        first = false;
        out << escapeCSV(cell.getText(), delim);
      }
      out << "\n";
    }
  }
}

} // namespace writers
} // namespace tabula
