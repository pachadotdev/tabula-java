#include "tabula/detectors/NurminenDetectionAlgorithm.h"
#include "tabula/Page.h"
#include "tabula/Rectangle.h"

#ifdef HAVE_POPPLER
#include <algorithm>
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-page.h>

namespace tabula {
namespace detectors {

NurminenDetectionAlgorithm::NurminenDetectionAlgorithm() {}
NurminenDetectionAlgorithm::~NurminenDetectionAlgorithm() {}

// Simple box blur dilate for binary image represented as bytes
static void dilateBinary(std::vector<unsigned char> &img, int w, int h, int rx,
                         int ry) {
  std::vector<unsigned char> tmp(img.size(), 0);
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      bool any = false;
      for (int oy = std::max(0, y - ry); oy <= std::min(h - 1, y + ry) && !any;
           ++oy) {
        for (int ox = std::max(0, x - rx); ox <= std::min(w - 1, x + rx);
             ++ox) {
          if (img[oy * w + ox]) {
            any = true;
            break;
          }
        }
      }
      tmp[y * w + x] = any ? 255 : 0;
    }
  }
  img.swap(tmp);
}

// Connected components (4-connected) returning bounding boxes
static std::vector<tabula::Rectangle>
connectedComponentsBounds(const std::vector<unsigned char> &img, int w, int h) {
  std::vector<int> labels(w * h, -1);
  int next = 0;
  std::vector<int> minx, miny, maxx, maxy;
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      if (!img[y * w + x] || labels[y * w + x] != -1)
        continue;
      // flood fill
      int ix = next;
      minx.push_back(x);
      miny.push_back(y);
      maxx.push_back(x);
      maxy.push_back(y);
      std::vector<int> stack;
      stack.push_back(y * w + x);
      labels[y * w + x] = ix;
      while (!stack.empty()) {
        int idx = stack.back();
        stack.pop_back();
        int cy = idx / w, cx = idx % w;
        minx[ix] = std::min(minx[ix], cx);
        miny[ix] = std::min(miny[ix], cy);
        maxx[ix] = std::max(maxx[ix], cx);
        maxy[ix] = std::max(maxy[ix], cy);
        const int dx[4] = {-1, 1, 0, 0};
        const int dy[4] = {0, 0, -1, 1};
        for (int k = 0; k < 4; ++k) {
          int nx = cx + dx[k], ny = cy + dy[k];
          if (nx < 0 || nx >= w || ny < 0 || ny >= h)
            continue;
          int nidx = ny * w + nx;
          if (img[nidx] && labels[nidx] == -1) {
            labels[nidx] = ix;
            stack.push_back(nidx);
          }
        }
      }
      ++next;
    }
  }
  std::vector<tabula::Rectangle> out;
  for (int i = 0; i < next; ++i) {
    int tw = maxx[i] - minx[i] + 1;
    int th = maxy[i] - miny[i] + 1;
    out.emplace_back((float)miny[i], (float)minx[i], (float)tw, (float)th);
  }
  return out;
}

std::vector<tabula::Rectangle>
NurminenDetectionAlgorithm::detect(const tabula::Page &page) {
  // Render page to image using Poppler
  // Use a modest DPI to keep sizes reasonable
  int dpi = 150;
  (void)dpi;
  try {
    // poppler::page::render_to_image expects device resolution parameters in
    // some builds Use render_to_image which returns poppler::image Not all
    // poppler versions expose the same API; guard attempts We will attempt to
    // grab text/rulings fallback if render not available
  } catch (...) {
    (void)page;
    return {};
  }

  // Fallback approach: construct a bitmap from rulings and text chunks similar
  // to the OpenCV approach
  int width = (int)std::ceil(page.getWidth());
  int height = (int)std::ceil(page.getHeight());
  if (width <= 0 || height <= 0)
    return {};
  std::vector<unsigned char> img(width * height, 0);

  // draw rulings (white pixels)
  for (auto const &r : page.getUnprocessedRulings()) {
    if (r.vertical()) {
      int x = std::max(0, std::min(width - 1, (int)std::round(r.getLeft())));
      int y1 = std::max(0, std::min(height - 1, (int)std::round(r.getTop())));
      int y2 =
          std::max(0, std::min(height - 1, (int)std::round(r.getBottom())));
      for (int y = y1; y <= y2; ++y)
        img[y * width + x] = 255;
    } else if (r.horizontal()) {
      int y = std::max(0, std::min(height - 1, (int)std::round(r.getTop())));
      int x1 = std::max(0, std::min(width - 1, (int)std::round(r.getLeft())));
      int x2 = std::max(0, std::min(width - 1, (int)std::round(r.getRight())));
      for (int x = x1; x <= x2; ++x)
        img[y * width + x] = 255;
    }
  }

  // mark text chunks
  for (auto const &tc : page.getTextChunks()) {
    int x = std::max(0, std::min(width - 1, (int)std::round(tc.getLeft())));
    int y = std::max(0, std::min(height - 1, (int)std::round(tc.getTop())));
    img[y * width + x] = 255;
  }

  // dilate to close gaps (box kernel)
  dilateBinary(img, width, height, 8, 2);

  auto boxes = connectedComponentsBounds(img, width, height);
  // Filter small boxes
  std::vector<tabula::Rectangle> out;
  for (auto &b : boxes) {
    if (b.getWidth() < 20 || b.getHeight() < 20)
      continue;
    out.push_back(b);
  }
  return out;
}

} // namespace detectors
} // namespace tabula

#else

namespace tabula {
namespace detectors {

NurminenDetectionAlgorithm::NurminenDetectionAlgorithm() {}
NurminenDetectionAlgorithm::~NurminenDetectionAlgorithm() {}

std::vector<tabula::Rectangle>
NurminenDetectionAlgorithm::detect(const tabula::Page &page) {
  (void)page;
  return {};
}

} // namespace detectors
} // namespace tabula

#endif
