#pragma once
#include "tabula/Table.h"
#include "tabula/writers/Writer.h"
#include <ostream>
#include <string>
#include <vector>

namespace tabula {
namespace writers {

class CSVWriter : public Writer {
public:
  CSVWriter(char delimiter = ',', bool includePage = true,
            bool includeHeader = false)
      : delim(delimiter), includePageNumber(includePage),
        includeHeaderRow(includeHeader) {}
  void write(std::ostream &out, const Table &table) const;
  void write(std::ostream &out, const std::vector<Table> &tables) const;

private:
  char delim;
  bool includePageNumber;
  bool includeHeaderRow;
};

} // namespace writers
} // namespace tabula
