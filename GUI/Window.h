#pragma once
#include "GlobalWindow.h"

class Window: public GlobalWindow
{
public:
  ENGINE_API Window(Rect size);
  ENGINE_API ~Window();
private:
};