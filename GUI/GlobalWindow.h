#pragma once
#include "TexturedWidget.h"

class GlobalWindow: public Widget, public has_slots<>
{
public:
  ENGINE_API GlobalWindow(Rect size);
  ENGINE_API ~GlobalWindow();
  ENGINE_API void onDrop(Widget* w);
private:
};
