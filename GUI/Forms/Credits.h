#pragma once
#include "TexturedWidget.h"

class Credits: public Widget, public has_slots<>
{
public:
  Credits(Rect size);
  ~Credits();
private:
};
