#include "tabula/Cell.h"
#include <iostream>

int main() {
  using namespace tabula;
  Cell c(0, 0, 100, 10);
  c.addTextChunk(TextChunk("first", 1.0f, 0.0f));
  c.addTextChunk(TextChunk("second", 2.0f, 0.0f));
  c.addTextChunk(TextChunk("inline", 1.0f, 10.0f));
  std::string t = c.getText(true);
  std::cout << "Cell text with line returns: '" << t << "'" << std::endl;
  std::string t2 = c.getText(false);
  std::cout << "Cell text no line returns: '" << t2 << "'" << std::endl;
  return 0;
}
