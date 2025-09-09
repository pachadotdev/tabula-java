#include "tabula/tabula.h"
#include <cstring>

extern "C" {

const char *tabula_version() {
  static const char *v = "tabula-cpp-0.1";
  return v;
}

} // extern "C"
