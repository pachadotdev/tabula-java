#pragma once
#include "tabula/Rectangle.h"
#include "tabula/Ruling.h"
#include <algorithm>
#include <limits>
#include <numeric>
#include <vector>

namespace tabula {

class ProjectionProfile {
public:
  static const int DECIMAL_PLACES = 1;
  ProjectionProfile(const Rectangle &area,
                    const std::vector<Rectangle> &elements,
                    float horizontalKernelSize, float verticalKernelSize);

  const std::vector<float> &getVerticalProjection() const {
    return verticalProjection_;
  }
  const std::vector<float> &getHorizontalProjection() const {
    return horizontalProjection_;
  }

  static std::vector<int>
  findSeparatorsFromProjection(const std::vector<float> &derivative);
  static std::vector<float> smooth(const std::vector<float> &data,
                                   int kernelSize);
  static std::vector<float> filter(const std::vector<float> &data, float alpha);
  static std::vector<float> getFirstDeriv(const std::vector<float> &projection);
  // return separators in absolute page coordinates (floats)
  std::vector<float> findVerticalSeparators() const;
  std::vector<float> findHorizontalSeparators() const;
  // parameterized versions that filter separators closer than the given
  // minimum column/row size (in same units as area coordinates)
  std::vector<float> findVerticalSeparators(float minColumnWidth) const;
  std::vector<float> findHorizontalSeparators(float minRowHeight) const;
  // Java-parity overloads: accept explicit rulings extracted from the Page
  std::vector<float>
  findVerticalSeparators(float minColumnWidth,
                         const std::vector<Ruling> &explicitRulings) const;
  std::vector<float>
  findHorizontalSeparators(float minRowHeight,
                           const std::vector<Ruling> &explicitRulings) const;

private:
  Rectangle area_;
  Rectangle textBounds_;
  std::vector<float> verticalProjection_;
  std::vector<float> horizontalProjection_;
  double areaWidth_, areaHeight_, areaTop_, areaLeft_;
  float minCharWidth_, minCharHeight_, horizontalKernelSize_,
      verticalKernelSize_;
  float maxHorizontalProjection_ = 0, maxVerticalProjection_ = 0;
  int toFixed(double value) const {
    return static_cast<int>(std::round(value * std::pow(10, DECIMAL_PLACES)));
  }
  double toDouble(int value) const {
    return value / std::pow(10, DECIMAL_PLACES);
  }
  void addRectangle(const Rectangle &element);
};

} // namespace tabula
