#include "tabula/Cell.h"
#include "tabula/Table.h"
#include "tabula/writers/CSVWriter.h"
#include <iostream>
#include <sstream>

int main() {
  using namespace tabula;
  using namespace tabula::writers;

  Table t;
  t.setRect(Rectangle(0, 0, 100, 100));
  t.setPageNumber(42);
  t.setColumnHeaders(std::vector<std::string>{"c1", "c2"});

  Cell a(0, 0, 50, 10);
  TextChunk ta("X", 0, 0);
  a.addTextChunk(ta);
  t.add(a, 0, 0);

  // CSV no page, no header
  {
    std::ostringstream cs;
    CSVWriter cw(',', false, false);
    cw.write(cs, t);
    std::string csv = cs.str();
    if (csv.find("42") != std::string::npos) {
      std::cerr << "page present when it should be omitted" << std::endl;
      return 1;
    }
  }

  // CSV with header but no page
  {
    std::ostringstream cs;
    CSVWriter cw(',', false, true);
    cw.write(cs, t);
    std::string csv = cs.str();
    if (csv.find("c1") == std::string::npos) {
      std::cerr << "header missing" << std::endl;
      return 1;
    }
    if (csv.find("42") != std::string::npos) {
      std::cerr << "page present when it should be omitted" << std::endl;
      return 1;
    }
  }

  std::cout << "writers_flags OK" << std::endl;
  return 0;
}
