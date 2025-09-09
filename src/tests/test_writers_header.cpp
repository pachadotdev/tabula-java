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
  t.setPageNumber(3);
  t.setColumnHeaders(std::vector<std::string>{"col1", "col2"});

  // create 1 row x 2 cols
  Cell a(0, 0, 50, 10);
  TextChunk ta("A", 0, 0);
  a.addTextChunk(ta);
  Cell b(0, 50, 50, 10);
  TextChunk tb("B", 0, 50);
  b.addTextChunk(tb);
  t.add(a, 0, 0);
  t.add(b, 0, 1);

  // CSV with header and page
  std::ostringstream cs;
  CSVWriter cw(',', true, true); // include page, include header
  cw.write(cs, t);
  std::string csv = cs.str();
  // first line should contain page header and our column headers
  if (csv.find("page") == std::string::npos ||
      csv.find("col1") == std::string::npos ||
      csv.find("col2") == std::string::npos) {
    std::cerr << "CSV header missing/page header" << std::endl;
    return 1;
  }

  // ensure data row contains page number and cell text
  if (csv.find("3") == std::string::npos ||
      csv.find("A") == std::string::npos ||
      csv.find("B") == std::string::npos) {
    std::cerr << "CSV data missing page or cells" << std::endl;
    return 1;
  }

  std::cout << "writers_header OK" << std::endl;
  return 0;
}
