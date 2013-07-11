#include "StdAfx.h"
#include "MyB2Draw.h"

MyB2Draw::MyB2Draw():renderer_(Renderer::getInstance())
{
  zoom_ = 0.1f;
  desiredZoom_ = 0.1f;
  minZoom_ = 0.001f;
}

MyB2Draw::~MyB2Draw()
{

}

FCoord MyB2Draw::getCoord(const b2Vec2& coord)
{
  FCoord res;
  res.x = (coord.x / renderer_.getAspect() + offsetX_) * zoom_;
  res.y = 1.0f-(coord.y - offsetY_) * zoom_;
  return res;
}

void MyB2Draw::DrawPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& color )
{
  renderer_.setColor(Vector3(color.r*255.0f, color.g*255.0f, color.b*255.0f));

  for (int i=0; i<vertexCount-1; ++i) {
    const b2Vec2& v1 = vertices[i];
    const b2Vec2& v2 = vertices[i+1];
    static int aaa = 0;
    ++aaa;
    renderer_.drawLine(getCoord(v1), getCoord(v2));
  }

  renderer_.drawLine(getCoord(vertices[vertexCount-1]), getCoord(vertices[0]));

  renderer_.resetColor();
}

void MyB2Draw::DrawSolidPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& color )
{
  DrawPolygon(vertices, vertexCount, color);
}

void MyB2Draw::DrawCircle( const b2Vec2& center, float32 radius, const b2Color& color )
{
  renderer_.setColor(Vector3(color.r*255.0f, color.g*255.0f, color.b*255.0f));

  renderer_.drawCircle(getCoord(center), radius*zoom_/renderer_.getAspect(), 30);

  renderer_.resetColor();
}

void MyB2Draw::DrawSolidCircle( const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color )
{
  DrawCircle(center, radius, color);
  renderer_.drawLine(getCoord(center), getCoord(center + radius * axis));
}

void MyB2Draw::DrawSegment( const b2Vec2& p1, const b2Vec2& p2, const b2Color& color )
{
  renderer_.setColor(Vector3(color.r*255.0f, color.g*255.0f, color.b*255.0f));
  renderer_.drawLine(getCoord(p1), getCoord(p2));
  renderer_.resetColor();
}

void MyB2Draw::DrawTransform( const b2Transform& xf )
{
  b2Vec2 p1 = xf.p, p2;
  const float32 k_axisScale = 0.4f;

  
  p2 = p1 + k_axisScale * xf.q.GetXAxis();
  renderer_.drawLine(getCoord(p1), getCoord(p2));
  p2 = p1 + k_axisScale * xf.q.GetYAxis();
  renderer_.drawLine(getCoord(p1), getCoord(p2));


}

void MyB2Draw::setOffset( float x, float y )
{
  offsetX_ = 0.5f/zoom_ - x/renderer_.getAspect(); 
  offsetY_ = y - 0.5f/zoom_;
}
