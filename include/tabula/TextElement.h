#pragma once
#include "tabula/HasText.h"
#include "tabula/Rectangle.h"
#include <string>

namespace tabula {

struct TextElement : public Rectangle, public HasText {
  std::string text;
  float fontSize;
  float widthOfSpace;
  float dir;

  TextElement() : Rectangle(), text(), fontSize(0), widthOfSpace(0), dir(0) {}
  TextElement(float y, float x, float width, float height, const std::string &c,
              float widthOfSpace_, float dir_ = 0.0f)
      : Rectangle(y, x, width, height), text(c), fontSize(0),
        widthOfSpace(widthOfSpace_), dir(dir_) {}

  std::string getText() const override { return text; }
  std::string getText(bool) const override { return text; }

  float getDirection() const { return dir; }
  float getWidthOfSpace() const { return widthOfSpace; }
  float getFontSize() const { return fontSize; }
};

} // namespace tabula
