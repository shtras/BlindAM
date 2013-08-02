#pragma once
#include "TexturedWidget.h"
#include "Button.h"

class TimeControl: public Widget, public has_slots<>
{
public:
  TimeControl(Rect size);
  ~TimeControl();
private:
  void pauseButtonClick();
  Button* pauseButton_;
};
