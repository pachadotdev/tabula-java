#pragma once
#include "tabula/Table.h"
#include "tabula/writers/Writer.h"
#include <ostream>
#include <string>
#include <vector>

namespace tabula {
namespace writers {

class JSONWriter : public Writer {
public:
  void write(std::ostream &out, const Table &table) const;
  void write(std::ostream &out, const std::vector<Table> &tables) const;
};

} // namespace writers
} // namespace tabula
