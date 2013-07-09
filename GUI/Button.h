#pragma once
#include "Text.h"

class Button: public Text
{
public:
  ENGINE_API Button(Rect size, bool defaultTexture = true);
  ENGINE_API ~Button();
  signal0<> sigClick;
  signal1<void*> sig1Click;
  ENGINE_API void onClick();
  ENGINE_API void init(TextureParams regular, TextureParams hovered, TextureParams pressed);
  ENGINE_API void setParam(void* param) {param_ = param;}
private:
  void* param_;
};
