#include "tabula/TextChunk.h"
#include "tabula/TextElement.h"
#include <cassert>
#include <iostream>

using namespace tabula;

int main() {
  // Single TextChunk composed of one TextElement containing a run of '-'
  TextElement te(0, 0, 100, 10, std::string("aaa---bbb"), 1.0f);
  TextChunk tc;
  tc.add(te);

  auto parts = tc.squeeze('-', 3);
  // Expect two parts: "aaa" and "bbb"
  assert(parts.size() == 2);
  std::string a = parts[0].getText();
  std::string b = parts[1].getText();
  assert(a == "aaa");
  assert(b == "bbb");
  std::cout << "test_textchunk_squeeze: OK (parts=" << parts.size() << ")\n";
  return 0;
}
