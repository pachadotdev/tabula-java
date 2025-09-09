#pragma once
#include <iostream>
#include <string>

namespace tabula {

// Simple diagnostics control. Default false; set to true in the CLI when
// verbose debugging is desired.
extern bool diagnostics_enabled;

inline void diag(const std::string &s) {
  if (diagnostics_enabled) std::cerr << s << std::endl;
}

} // namespace tabula
