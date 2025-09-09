#pragma once
#include <string>

namespace tabula {

struct HasText {
  virtual ~HasText() = default;
  virtual std::string getText() const = 0;
  virtual std::string getText(bool useLineReturns) const = 0;
};

} // namespace tabula
