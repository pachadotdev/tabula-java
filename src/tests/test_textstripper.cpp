#include "tabula/TextElement.h"
#include "tabula/TextStripper.h"
#include <iostream>

int main() {
  using namespace tabula;
  std::vector<TextElement> elems;
  elems.emplace_back(0.0f, 0.0f, 10.0f, 5.0f, "Hello", 0.0f);
  elems.emplace_back(0.0f, 12.0f, 8.0f, 5.0f, "World", 0.0f);
  elems.emplace_back(10.0f, 0.0f, 5.0f, 5.0f, "X", 0.0f);

  TextStripper ts(3.0f);
  auto chunks = ts.strip(elems);
  if (chunks.size() != 2)
    return 2; // first line Hello World, second line X
  std::cout << "textstripper OK\n";
  return 0;
}
