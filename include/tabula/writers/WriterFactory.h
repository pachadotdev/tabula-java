// Simple factory for creating Writer instances by name
#pragma once
#include "tabula/writers/Writer.h"
#include <memory>
#include <string>

namespace tabula {
namespace writers {

std::unique_ptr<Writer> createWriter(const std::string &format,
                                     char delim = ',', bool includePage = true,
                                     bool includeHeader = false);

}
} // namespace tabula
