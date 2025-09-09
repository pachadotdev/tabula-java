#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#ifdef HAVE_POPPLER
#include "tabula/pages.h"
#include "tabula/poppler_integration.h"
#include "tabula/writers/CSVWriter.h"
#include "tabula/writers/JSONWriter.h"
#endif

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: tabula [--format json|csv] [--delimiter ,] "
                 "[--no-page] [--header] [--pages 1-3,5] [--area "
                 "left,top,width,height] [--output file] <pdf-file>"
              << std::endl;
    return 2;
  }

  std::string format = "json";
  char delim = ',';
  bool includePage = true;
  bool includeHeader = false;
  std::string pagesSpec;
  std::string areaSpec;
  std::string outputPath;
  std::string path;

  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--format" && i + 1 < argc) {
      format = argv[++i];
    } else if (a == "--delimiter" && i + 1 < argc) {
      delim = argv[++i][0];
    } else if (a == "--no-page") {
      includePage = false;
    } else if (a == "--header") {
      includeHeader = true;
    } else if (a == "--pages" && i + 1 < argc) {
      pagesSpec = argv[++i];
    } else if (a == "--area" && i + 1 < argc) {
      areaSpec = argv[++i];
    } else if (a == "--output" && i + 1 < argc) {
      outputPath = argv[++i];
    } else if (a.rfind("--", 0) == 0) {
      std::cerr << "Unknown option: " << a << std::endl;
      return 2;
    } else {
      path = a;
    }
  }

  if (path.empty()) {
    std::cerr << "Missing PDF path" << std::endl;
    return 2;
  }

#ifdef HAVE_POPPLER
  // Delegate to poppler integration code path (reuse its logic)
  std::vector<std::string> args;
  args.push_back("tabula-cpp");
  if (format == "csv") {
    args.push_back("--format");
    args.push_back("csv");
    args.push_back("--delimiter");
    args.push_back(std::string(1, delim));
  }
  if (!includePage)
    args.push_back("--no-page");
  if (includeHeader)
    args.push_back("--header");
  if (!pagesSpec.empty()) {
    args.push_back("--pages");
    args.push_back(pagesSpec);
  }
  if (!areaSpec.empty()) {
    args.push_back("--area");
    args.push_back(areaSpec);
  }
  args.push_back(path);
  auto pages = tabula::parsePagesSpec(pagesSpec);
  // If outputPath specified, redirect std::cout temporarily
  if (!outputPath.empty()) {
    std::ofstream fout(outputPath, std::ios::binary);
    if (!fout) {
      std::cerr << "Failed to open output file: " << outputPath << std::endl;
      return 2;
    }
    auto *oldbuf = std::cout.rdbuf(fout.rdbuf());
    int rc = poppler_integration_run(args, pages);
    std::cout.rdbuf(oldbuf);
    return rc;
  }
  return poppler_integration_run(args, pages);
#else
  // silence unused variable warnings when built without Poppler
  (void)delim;
  (void)includePage;
  (void)includeHeader;
  (void)format;
  std::cerr << "tabula-cli: built without Poppler support. Configure with "
               "-DUSE_XPDF=ON and install poppler-cpp."
            << std::endl;
  return 2;
#endif
}
