#include "tabula/Cell.h"
#include "tabula/Table.h"
#include "tabula/writers/CSVWriter.h"
#include "tabula/writers/JSONWriter.h"
#include <iostream>
#include <sstream>

int main() {
  using namespace tabula;
  using namespace tabula::writers;
  Table t;
  t.setRect(Rectangle(0, 0, 100, 100));
  t.setPageNumber(1);
  // create 1 row x 2 cols
  Cell a(0, 0, 50, 10);
  TextChunk ta("A", 0, 0);
  a.addTextChunk(ta);
  Cell b(0, 50, 50, 10);
  TextChunk tb("B", 0, 50);
  b.addTextChunk(tb);
  t.add(a, 0, 0);
  t.add(b, 0, 1);

  // CSV
  std::ostringstream cs;
  CSVWriter cw(',');
  cw.write(cs, t);
  std::string csv = cs.str();
  if (csv.find("A") == std::string::npos ||
      csv.find("B") == std::string::npos) {
    std::cerr << "CSV output missing cells" << std::endl;
    return 1;
  }

  // JSON
  std::ostringstream js;
  JSONWriter jw;
  jw.write(js, std::vector<Table>{t});
  std::string j = js.str();
  if (j.find("\"data\"") == std::string::npos ||
      j.find("A") == std::string::npos) {
    std::cerr << "JSON output missing data" << std::endl;
    return 1;
  }
  if (j.find("\"page_number\":1") == std::string::npos) {
    std::cerr << "JSON output missing per-cell page_number" << std::endl;
    return 1;
  }

  std::cout << "writers OK" << std::endl;
  return 0;
}
