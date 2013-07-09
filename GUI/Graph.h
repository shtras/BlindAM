#pragma once
#include "TexturedWidget.h"

class Graph: public Widget
{
public:
  ENGINE_API Graph(Rect size, int maxValues, int numGraphs);
  ENGINE_API ~Graph();
  ENGINE_API void addValue(int graph, float value);
  ENGINE_API void render();
  ENGINE_API void setColor(int graph, Vector3 color);
private:
  void recalcMax();
  uint32_t maxValues_;
  list<float>* values_;
  Vector3* graphColor_;
  float max_;
  int numGraphs_;
};
