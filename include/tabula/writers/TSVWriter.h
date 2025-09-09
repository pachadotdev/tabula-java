#pragma once
#include "tabula/writers/CSVWriter.h"

namespace tabula {
namespace writers {

class TSVWriter : public CSVWriter {
public:
  TSVWriter() : CSVWriter('\t') {}
};

} // namespace writers
} // namespace tabula
