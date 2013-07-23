#pragma once
#include "Box2D/Box2D.h"
#include "Renderer.h"
#include "Scrollable.h"

class MyB2Draw: public b2Draw, public Scrollable
{
public:
  MyB2Draw();
  ~MyB2Draw();

  void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

  void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

  void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

  void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

  void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

  void DrawTransform(const b2Transform& xf);
  void setOffset(float x, float y);
  float getLeft();
  float getZoom() {return zoom_;}
private:
  FCoord getCoord(const b2Vec2& coord);
  Renderer& renderer_;
};
