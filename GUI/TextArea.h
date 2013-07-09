#pragma once
#include "TexturedWidget.h"
#include "ProgressBar.h"

class TextArea: public Widget
{
public:
  ENGINE_API TextArea(Rect size);
  ENGINE_API ~TextArea();
  ENGINE_API virtual void onResize();
  ENGINE_API void pushLine(CString line);
  ENGINE_API void onMouseWheelScroll(int direction);
  ENGINE_API void render();
  ENGINE_API void clearLines();
  ENGINE_API void setTextSize(float value);
private:
  void resizePB();
  list<CString> lines_;
  int maxSize_;
  int first_;
  int maxLinesDisplayed_;
  ProgressBar* pb_;
  float textSize_;
};
