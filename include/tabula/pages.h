// Parse a pagespec like "1-3,5,7" into zero-based page indices
#pragma once
#include <string>
#include <vector>

namespace tabula {
inline std::vector<int> parsePagesSpec(const std::string &spec) {
  std::vector<int> out;
  if (spec.empty())
    return out;
  size_t i = 0;
  while (i < spec.size()) {
    // read number
    size_t j = i;
    while (j < spec.size() && isdigit((unsigned char)spec[j]))
      j++;
    if (j == i) {
      i++;
      continue;
    }
    int a = std::stoi(spec.substr(i, j - i));
    if (j < spec.size() && spec[j] == '-') {
      size_t k = j + 1;
      while (k < spec.size() && isdigit((unsigned char)spec[k]))
        k++;
      if (k == j + 1) {
        i = k;
        continue;
      }
      int b = std::stoi(spec.substr(j + 1, k - (j + 1)));
      for (int p = a; p <= b; ++p)
        out.push_back(p - 1);
      i = k;
    } else {
      out.push_back(a - 1);
      i = j;
    }
    if (i < spec.size() && spec[i] == ',')
      ++i;
  }
  return out;
}
} // namespace tabula
