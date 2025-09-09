#include <iostream>

int main(int argc, char **argv) {
  std::cout << "tabula-cpp prototype: no PDF functionality yet\n";
  if (argc > 1) {
    std::cout << "args:";
    for (int i = 1; i < argc; ++i)
      std::cout << ' ' << argv[i];
    std::cout << '\n';
  }
  return 0;
}
