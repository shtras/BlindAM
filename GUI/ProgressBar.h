#pragma once
#include "TexturedWidget.h"

class ProgressBar: public Widget
{
public:
  ENGINE_API ProgressBar(Rect size);
  ENGINE_API ~ProgressBar();
  ENGINE_API void setParams(int numItems, int firstItem, int lastItem);
  ENGINE_API void customFrameRender();
private:
  TexturedWidget* bg_;
  TexturedWidget* selection_;
  int numItems_;
  int firstItem_;
  int lastItem_;
};
