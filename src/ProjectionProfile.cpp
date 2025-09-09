#include "tabula/ProjectionProfile.h"
#include <cmath>

namespace tabula {

// helpers mirroring Java static methods used by ProjectionProfile
std::vector<float> ProjectionProfile::smooth(const std::vector<float> &data,
                                             int kernelSize) {
  std::vector<float> rv(data.size(), 0.0f);
  if (kernelSize <= 0)
    return rv;
  for (size_t i = 0; i < data.size(); ++i) {
    float s = 0.0f;
    int start = std::max(0, static_cast<int>(i) - kernelSize / 2);
    int end = std::min(static_cast<int>(data.size()),
                       static_cast<int>(i) + kernelSize / 2);
    for (int j = start; j < end; ++j)
      s += data[j];
    rv[i] = std::floor(s / kernelSize);
  }
  return rv;
}

std::vector<float> ProjectionProfile::filter(const std::vector<float> &data,
                                             float alpha) {
  if (data.empty())
    return {};
  std::vector<float> rv(data.size());
  rv[0] = data[0];
  for (size_t i = 1; i < data.size(); ++i)
    rv[i] = rv[i - 1] + alpha * (data[i] - rv[i - 1]);
  return rv;
}

std::vector<float>
ProjectionProfile::getFirstDeriv(const std::vector<float> &projection) {
  if (projection.empty())
    return {};
  std::vector<float> rv(projection.size(), 0.0f);
  if (projection.size() > 1)
    rv[0] = projection[1] - projection[0];
  for (size_t i = 1; i + 1 < projection.size(); ++i)
    rv[i] = projection[i + 1] - projection[i - 1];
  if (projection.size() > 1)
    rv[projection.size() - 1] =
        projection[projection.size() - 1] - projection[projection.size() - 2];
  return rv;
}

std::vector<int> ProjectionProfile::findSeparatorsFromProjection(
    const std::vector<float> &derivative) {
  std::vector<int> separators;
  int lastNeg = -1;
  bool positiveSlope = false;
  for (int i = 0; i < static_cast<int>(derivative.size()); ++i) {
    float s = derivative[i];
    if (s > 0 && !positiveSlope) {
      positiveSlope = true;
      separators.push_back(lastNeg != -1 ? lastNeg : i);
    } else if (s < 0) {
      lastNeg = i;
      positiveSlope = false;
    }
  }
  return separators;
}

ProjectionProfile::ProjectionProfile(const Rectangle &area,
                                     const std::vector<Rectangle> &elements,
                                     float horizontalKernelSize,
                                     float verticalKernelSize)
    : area_(area), areaWidth_(area.getWidth()), areaHeight_(area.getHeight()),
      areaTop_(area.getTop()), areaLeft_(area.getLeft()),
      horizontalKernelSize_(horizontalKernelSize),
      verticalKernelSize_(verticalKernelSize) {
  textBounds_ = area;
  verticalProjection_.assign(toFixed(areaHeight_), 0.0f);
  horizontalProjection_.assign(toFixed(areaWidth_), 0.0f);
  minCharHeight_ = std::numeric_limits<float>::max();
  minCharWidth_ = std::numeric_limits<float>::max();

  for (const auto &element : elements) {
    if (element.getWidth() / textBounds_.getWidth() > 0.8f)
      continue;
    addRectangle(element);
  }

  verticalProjection_ =
      smooth(verticalProjection_, toFixed(verticalKernelSize_));
  horizontalProjection_ =
      smooth(horizontalProjection_, toFixed(horizontalKernelSize_));
}

void ProjectionProfile::addRectangle(const Rectangle &element) {
  if (!area_.contains(element))
    return;
  minCharHeight_ = std::min(minCharHeight_, element.getHeight());
  minCharWidth_ = std::min(minCharWidth_, element.getWidth());
  for (int k = toFixed(element.getLeft()); k < toFixed(element.getRight());
       ++k) {
    int idx = k - toFixed(areaLeft_);
    if (idx >= 0 && idx < static_cast<int>(horizontalProjection_.size())) {
      horizontalProjection_[idx] += element.getHeight();
      maxHorizontalProjection_ =
          std::max(maxHorizontalProjection_, horizontalProjection_[idx]);
    }
  }
  for (int k = toFixed(element.getTop()); k < toFixed(element.getBottom());
       ++k) {
    int idx = k - toFixed(areaTop_);
    if (idx >= 0 && idx < static_cast<int>(verticalProjection_.size())) {
      verticalProjection_[idx] += element.getWidth();
      maxVerticalProjection_ =
          std::max(maxVerticalProjection_, verticalProjection_[idx]);
    }
  }
}

std::vector<float> ProjectionProfile::findVerticalSeparators() const {
  return findVerticalSeparators(minCharWidth_);
}

std::vector<float> ProjectionProfile::findHorizontalSeparators() const {
  return findHorizontalSeparators(minCharHeight_);
}

std::vector<float>
ProjectionProfile::findVerticalSeparators(float minColumnWidth) const {
  return findVerticalSeparators(minColumnWidth, std::vector<Ruling>());
}

std::vector<float>
ProjectionProfile::findHorizontalSeparators(float minRowHeight) const {
  return findHorizontalSeparators(minRowHeight, std::vector<Ruling>());
}

std::vector<float> ProjectionProfile::findVerticalSeparators(
    float minColumnWidth, const std::vector<Ruling> &explicitRulings) const {
  std::vector<int> verticalSeparators;
  // Add explicit full-length vertical rulings similar to Java
  for (const auto &r : explicitRulings) {
    if (r.vertical()) {
      if (r.length() / areaHeight_ >= 0.95)
        verticalSeparators.push_back(toFixed(r.getPosition() - areaLeft_));
    }
  }

  // derivative of vertical projection (height-wise) -> horizontal separators
  auto deriv = getFirstDeriv(verticalProjection_);
  auto seps = findSeparatorsFromProjection(filter(deriv, 0.1f));

  for (int foundSep : seps) {
    bool foundNarrower = false;
    for (int explicitSep : verticalSeparators) {
      if (std::abs(toDouble(foundSep - explicitSep)) <= minColumnWidth) {
        foundNarrower = true;
        break;
      }
    }
    if (!foundNarrower)
      verticalSeparators.push_back(foundSep);
  }
  std::sort(verticalSeparators.begin(), verticalSeparators.end());
  std::vector<float> rv;
  rv.reserve(verticalSeparators.size());
  for (int id : verticalSeparators)
    rv.push_back(static_cast<float>(toDouble(id) + areaLeft_));
  return rv;
}

std::vector<float> ProjectionProfile::findHorizontalSeparators(
    float minRowHeight, const std::vector<Ruling> &explicitRulings) const {
  std::vector<int> horizontalSeparators;
  for (const auto &r : explicitRulings) {
    if (r.horizontal()) {
      if (r.length() / areaWidth_ >= 0.95)
        horizontalSeparators.push_back(toFixed(r.getPosition() - areaTop_));
    }
  }

  auto deriv = getFirstDeriv(horizontalProjection_);
  auto seps = findSeparatorsFromProjection(filter(deriv, 0.1f));

  for (int foundSep : seps) {
    bool foundShorter = false;
    for (int explicitSep : horizontalSeparators) {
      if (std::abs(toDouble(foundSep - explicitSep)) <= minRowHeight) {
        foundShorter = true;
        break;
      }
    }
    if (!foundShorter)
      horizontalSeparators.push_back(foundSep);
  }
  std::sort(horizontalSeparators.begin(), horizontalSeparators.end());
  std::vector<float> rv;
  rv.reserve(horizontalSeparators.size());
  for (int id : horizontalSeparators)
    rv.push_back(static_cast<float>(toDouble(id) + areaTop_));
  return rv;
}

} // namespace tabula

namespace tabula {
// All methods are inline in the header for now.
}
