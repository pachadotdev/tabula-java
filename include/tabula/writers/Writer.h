#pragma once
#include <ostream>
#include <vector>

namespace tabula {
class Table;
}

namespace tabula {
namespace writers {

class Writer {
public:
  virtual ~Writer() {}
  virtual void write(std::ostream &out,
                     const std::vector<class Table> &tables) const = 0;
  // concrete writers may provide a convenience single-Table overload if desired
};

} // namespace writers
} // namespace tabula
