#pragma once
#include "TexturedWidget.h"

class Text: public Widget
{
public:
  ENGINE_API Text(Rect size);
  ENGINE_API ~Text();
  ENGINE_API void render();
  ENGINE_API void setCaption(CString caption) {caption_ = caption;}
  ENGINE_API void setTextSize(float size) {textSize_ = size;}
  ENGINE_API void setColor(Vector4 color) {color_ = color;}
protected:  
  CString caption_;
  float textSize_;
  Vector4 color_;
};

class FramedText: public Text
{
public:
  ENGINE_API FramedText(Rect size);
  ENGINE_API ~FramedText();
private:
};
