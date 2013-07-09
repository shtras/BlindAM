#pragma once

class Scrollable
{
public:
  ENGINE_API Scrollable();
  ENGINE_API virtual ~Scrollable();
  ENGINE_API void scrollMDown();
  ENGINE_API void scrollMUp();
  ENGINE_API void scrollMMove();
  ENGINE_API void scrollMScroll(int direction);
  ENGINE_API bool scrollZoomStep();
protected:
  bool scrolling_;
  float lastMouseX_;
  float lastMouseY_;
  float offsetX_;
  float offsetY_;
  float zoom_;
  float desiredZoom_;
  float zoomStep_;
  float maxZoom_;
  float minZoom_;
};
