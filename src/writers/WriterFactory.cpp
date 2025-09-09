#include "tabula/writers/WriterFactory.h"
#include "tabula/writers/CSVWriter.h"
#include "tabula/writers/JSONWriter.h"

namespace tabula {
namespace writers {

std::unique_ptr<Writer> createWriter(const std::string &format, char delim,
                                     bool includePage, bool includeHeader) {
  if (format == "csv") {
    return std::unique_ptr<Writer>(
        new CSVWriter(delim, includePage, includeHeader));
  }
  return std::unique_ptr<Writer>(new JSONWriter());
}

} // namespace writers
} // namespace tabula
