#pragma once
#include "Text.h"

class Input: public FramedText
{
public:
  ENGINE_API Input(Rect size);
  ENGINE_API ~Input();
  ENGINE_API void onKeyDown(SDL_keysym details);
  ENGINE_API void onKeyboardFocusGain();
  ENGINE_API void onKeyboardFocusLose();
  ENGINE_API CString getCaption();
private:
  bool inFocus_;
};
