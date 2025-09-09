#include "tabula/writers/JSONWriter.h"
#include "tabula/RectangularTextContainer.h"
#include "tabula/json/Serializers.h"

namespace tabula {
namespace writers {

void JSONWriter::write(std::ostream &out, const Table &table) const {
  write(out, std::vector<Table>{table});
}

void JSONWriter::write(std::ostream &out,
                       const std::vector<Table> &tables) const {
  out << "[";
  bool firstTable = true;
  for (auto const &t : tables) {
    if (!firstTable)
      out << ",";
    firstTable = false;
    out << tabula::json::serializeTable(t);
  }
  out << "]";
}

} // namespace writers
} // namespace tabula
