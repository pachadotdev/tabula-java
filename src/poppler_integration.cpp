#ifdef HAVE_POPPLER
#include "tabula/ObjectExtractor.h"
#include "tabula/SpreadsheetExtractionAlgorithm.h"
#include "tabula/BasicExtractionAlgorithm.h"
#include "tabula/TextElement.h"
#include "tabula/writers/CSVWriter.h"
#include "tabula/writers/JSONWriter.h"
#include "tabula/writers/WriterFactory.h"
#ifdef HAVE_POPPLER
#include "tabula/ObjectExtractorPopplerEngine.h"
#endif
#include "tabula/Diagnostics.h"
#include <iostream>
#include <sstream>
#include <memory>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

using namespace tabula;

// Map Poppler page text into TextElement vector using
// poppler::page::text_list()
static std::vector<TextElement> pageToTextElements(poppler::page &p) {
  std::vector<TextElement> out;
  auto text_list = p.text_list();
  float page_h = 0.0f;
  try {
    auto rect = p.page_rect();
    page_h = static_cast<float>(rect.height());
  } catch (...) {
  }
  for (const auto &item : text_list) {
    // item is poppler::text_box (or text_box/line) - use text() and bounding
    // box
    try {
      auto ba = item.text().to_utf8();
      std::string s(ba.begin(), ba.end());
  auto r = item.bbox();
  // Poppler uses a coordinate system with origin at bottom-left; convert
  // to our top-left origin by flipping Y using the page height.
  float left = static_cast<float>(r.left());
  float height = static_cast<float>(r.height());
  float raw_top = static_cast<float>(r.top());
  float top = (page_h > 0.0f) ? (page_h - (raw_top + height)) : raw_top;
  float width = static_cast<float>(r.width());
      // approximate width-of-space as 0.25 * font height; poppler doesn't
      // expose it easily
      float widthOfSpace = height * 0.25f;
  out.emplace_back(top, left, width, height, s, widthOfSpace);
    } catch (...) {
      // ignore any item we can't process
    }
  }
  return out;
}

// Diagnostics flag default
bool tabula::diagnostics_enabled = false;

int poppler_integration_run(const std::vector<std::string> &args,
                            const std::vector<int> &pages) {
  if (args.size() < 2) {
    std::cerr << "Usage: poppler-integration [--format json|csv] [--delimiter "
                 ",] [--no-page] [--header] <pdf-file>"
              << std::endl;
    return 2;
  }

  // Simple CLI parsing
  std::string format = "json";
  char delim = ',';
  bool includePage = true;
  bool includeHeader = false;
  std::string pagesSpec;
  std::string path;

  for (size_t i = 1; i < args.size(); ++i) {
    std::string a = args[i];
    if (a == "--debug") {
      tabula::diagnostics_enabled = true;
      continue;
    }
    if (a == "--format" && i + 1 < args.size()) {
      format = args[++i];
    } else if (a == "--delimiter" && i + 1 < args.size()) {
      delim = args[++i][0];
    } else if (a == "--no-page") {
      includePage = false;
    } else if (a == "--header") {
      includeHeader = true;
    } else if (a == "--pages" && i + 1 < args.size()) {
      pagesSpec = args[++i];
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
  std::unique_ptr<poppler::document> doc(
      poppler::document::load_from_file(path));
  if (!doc) {
    std::cerr << "Failed to open PDF" << std::endl;
    return 1;
  }

  auto writer =
      tabula::writers::createWriter(format, delim, includePage, includeHeader);
  int np = doc->pages();
  std::vector<Table> allTables;
  auto shouldProcess = [&](int pageIndex) {
    if (pages.empty())
      return true;
    for (int p : pages)
      if (p == pageIndex)
        return true;
    return false;
  };
  for (int i = 0; i < np; ++i) {
    if (!shouldProcess(i + 1))
      continue;
    std::unique_ptr<poppler::page> p(doc->create_page(i));
    if (!p)
      continue;
  auto elements = pageToTextElements(*p);

  // Create a Poppler raster-based engine to detect ruling lines and pass
  // it to ObjectExtractor so rulings are available to the extraction
  // algorithms (mirrors Java ObjectExtractorStreamEngine behavior).
#ifdef HAVE_POPPLER
  tabula::ObjectExtractorPopplerEngine engine;
  try {
    engine.analyze(*p);
  } catch (...) {}
  ObjectExtractor oe(&engine, elements);
#else
  ObjectExtractor oe(nullptr, elements);
#endif
  Page page = oe.extractPage(i + 1);
    // Diagnostic: report number of text chunks and a sample
    try {
      const auto &tchunks = page.getTextChunks();
      tabula::diag(std::string("[diag] page ") + std::to_string(i + 1) + " textChunks=" + std::to_string(tchunks.size()));
      if (!tchunks.empty()) {
        std::string s = tchunks.front().getText();
        if (s.size() > 120) s = s.substr(0, 120) + "...";
        tabula::diag(std::string("[diag] sample chunk='") + s + "'");
      }
      // Dump first N TextElements (geometry + short text) for debugging
      try {
        size_t maxElems = 40;
        size_t printed = 0;
        for (const auto &tc : tchunks) {
          for (const auto &te : tc.getTextElements()) {
            if (printed >= maxElems) break;
            std::string t = te.getText();
            if (t.size() > 80) t = t.substr(0, 80) + "...";
            tabula::diag(std::string("[diag] TE top=") + std::to_string(te.getTop()) + " left=" + std::to_string(te.getLeft()) + " w=" + std::to_string(te.getWidth()) + " h=" + std::to_string(te.getHeight()) + " text=\"" + t + "\"");
            ++printed;
          }
          if (printed >= maxElems) break;
        }
      } catch (...) {}
    } catch (...) {}
    // Set page dimensions from Poppler so the ProjectionProfile and
    // TableDetector operate over the correct area (otherwise Page is
    // zero-sized and separators are empty).
    try {
      auto rect = p->page_rect();
      float pw = static_cast<float>(rect.width());
      float ph = static_cast<float>(rect.height());
      page.setLeft(0.0f);
      page.setTop(0.0f);
      page.setRight(pw);
      page.setBottom(ph);
    } catch (...) {
      // ignore if poppler page rect can't be read
    }

    // First try spreadsheet (ruling-based) extraction. If nothing found,
    // fall back to a simpler detection-based extraction so PDFs without
    // explicit ruling lines still yield tables when possible.
    // If no unprocessed rulings exist, synthesize rulings from projection
    // separators (heuristic) so the ruling-based extractor can run.
    try {
      if (page.getUnprocessedRulings().empty()) {
        // choose a reasonable min column/row size; 2.0 is a sensible default
        auto vSeps = page.findVerticalSeparators(2.0f);
        auto hSeps = page.findHorizontalSeparators(2.0f);
        for (auto x : vSeps) {
          Ruling vr(static_cast<double>(x), page.getTop(), static_cast<double>(x), page.getBottom());
          page.addRuling(vr);
        }
        for (auto y : hSeps) {
          Ruling hr(page.getLeft(), static_cast<double>(y), page.getRight(), static_cast<double>(y));
          page.addRuling(hr);
        }
  tabula::diag(std::string("[diag] synthesized rulings v=") + std::to_string(vSeps.size()) + " h=" + std::to_string(hSeps.size()));
      }
    } catch (...) {}

    std::vector<Table> tables = SpreadsheetExtractionAlgorithm().extract(page);
    if (tables.empty()) {
      tables = BasicExtractionAlgorithm().extract(page);
    }
    
    for (auto &t : tables)
      allTables.push_back(std::move(t));
  }
  // If pagesSpec is provided, we would filter pages; for now pagesSpec is
  // ignored and we process all pages
  // Debug: print table structures and sample texts
  // Diagnostic dump: print each table's cell texts (length + short sample)
    for (size_t ti = 0; ti < allTables.size(); ++ti) {
    const auto &t = allTables[ti];
    {
      std::ostringstream oss;
      oss << "[diag] Table " << ti << " rows=" << t.getRowCount() << " cols=" << t.getColCount() << " rect=" << t.getRect();
      tabula::diag(oss.str());
    }
    const auto &rows = t.getRows();
    for (size_t r = 0; r < rows.size(); ++r) {
      for (size_t c = 0; c < rows[r].size(); ++c) {
        const auto &cell = rows[r][c];
        std::string txt = cell.getText();
        std::string sample = txt.substr(0, std::min<size_t>(50, txt.size()));
        if (txt.size() > 50)
          sample += "...";
        tabula::diag(std::string("[diag] cell(") + std::to_string(r) + "," + std::to_string(c) + ") len=" + std::to_string(txt.size()) + " sample='" + sample + "'");
      }
    }
  }

  writer->write(std::cout, allTables);
  return 0;
}

#else
#include <iostream>
int main() {
  std::cerr << "poppler-integration built without Poppler support" << std::endl;
  return 2;
}
#endif
