// NurminenDetectionAlgorithm stub - optional OpenCV-backed detector
#pragma once
#include "tabula/DetectionAlgorithm.h"
#include <vector>

namespace tabula {
namespace detectors {

class NurminenDetectionAlgorithm : public tabula::DetectionAlgorithm {
public:
  NurminenDetectionAlgorithm();
  ~NurminenDetectionAlgorithm() override;
  std::vector<tabula::Rectangle> detect(const tabula::Page &page) override;
  // Detection works directly from Page geometry or from a Poppler-rendered
  // image (no OpenCV needed).
};

} // namespace detectors
} // namespace tabula
