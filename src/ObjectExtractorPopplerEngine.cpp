#ifdef HAVE_POPPLER
#include "tabula/ObjectExtractorPopplerEngine.h"
#include "tabula/Rectangle.h"
#include "tabula/Diagnostics.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace tabula {

// Build rulings by clustering TextChunk geometry positions. This avoids
// relying on poppler render APIs and produces rulings where many chunks
// align along a vertical/horizontal position.
void ObjectExtractorPopplerEngine::analyze(poppler::page &p) {
  try {
    // Use text chunks from the page to propose ruling positions
    // We need a Page object to get text chunks; but poppler_integration
    // constructs ObjectExtractor with elements and then calls extractPage()
    // which uses engine_->getRulings(). To allow clustering we need to
    // access the poppler page directly: gather textual bounding boxes via
    // poppler::page::text_list() and map them to top/left/right/bottom.

    auto text_list = p.text_list();
    std::vector<double> xs;
    std::vector<double> ys;
    double page_h = 0.0, page_w = 0.0;
    try {
      auto rect = p.page_rect();
      page_h = rect.height();
      page_w = rect.width();
    } catch (...) {}
    for (const auto &item : text_list) {
      try {
        auto r = item.bbox();
        double left = static_cast<double>(r.left());
        double width = static_cast<double>(r.width());
        double top_raw = static_cast<double>(r.top());
        double height = static_cast<double>(r.height());
        double top = (page_h > 0.0) ? (page_h - (top_raw + height)) : top_raw;
        double right = left + width;
        double bottom = top + height;
        xs.push_back(left);
        xs.push_back(right);
        ys.push_back(top);
        ys.push_back(bottom);
      } catch (...) {}
    }

    auto cluster = [](std::vector<double> &vals, double tol) {
      std::vector<double> out;
      if (vals.empty()) return out;
      std::sort(vals.begin(), vals.end());
      double cur = vals.front();
      int count = 1;
      double sum = cur;
      for (size_t i = 1; i < vals.size(); ++i) {
        if (std::abs(vals[i] - cur) <= tol) {
          sum += vals[i]; ++count; cur = (cur * (count - 1) + vals[i]) / count;
        } else {
          if (count >= 3) out.push_back(sum / count);
          cur = vals[i]; count = 1; sum = vals[i];
        }
      }
      if (count >= 3) out.push_back(sum / count);
      return out;
    };

    double tol = 5.0; // points tolerance for clustering
    auto vx = cluster(xs, tol);
    auto vy = cluster(ys, tol);

    // Convert clusters to rulings spanning full page bounds
    for (double x : vx) {
      Ruling vr(x, 0.0, x, static_cast<double>(page_h > 0.0 ? page_h : 1000.0));
      this->addSegment(static_cast<float>(vr.getLeft()), static_cast<float>(vr.getTop()), static_cast<float>(vr.getRight()), static_cast<float>(vr.getBottom()));
    }
    for (double y : vy) {
      Ruling hr(0.0, y, static_cast<double>(page_w > 0.0 ? page_w : 1000.0), y);
      this->addSegment(static_cast<float>(hr.getLeft()), static_cast<float>(hr.getTop()), static_cast<float>(hr.getRight()), static_cast<float>(hr.getBottom()));
    }
  tabula::diag(std::string("[diag] engine clustered rulings vx=") + std::to_string(vx.size()) + " vy=" + std::to_string(vy.size()));
  } catch (const std::exception &e) {
  tabula::diag(std::string("[diag] engine analyze error: ") + e.what());
  } catch (...) {}
}

} // namespace tabula
#endif

