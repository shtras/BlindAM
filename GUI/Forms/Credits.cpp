#include "StdAfx.h"
#include "Credits.h"
#include "Button.h"
#include "BoxCar.h"

Credits::Credits( Rect size ):Widget(size)
{
  Button* back = new Button(Rect(0.45, 0.85, 0.1, 0.05));
  back->setCaption("Back");
  back->setColor(Vector4(255,0,0,255));
  back->sigClick.connect(&BoxCar::getInstance(), &BoxCar::toggleMenu);
  addWidget(back);

  double left = 0.2;
  double width = 0.6;
  double top = 0.1;
  double height = 0.02;
  Text* t = new Text(Rect(left, top, width, height));
  t->setCaption("Blind Auto Mechanic");
  t->setTextSize(3.0f);
  addWidget(t);
  top += height + 0.1;

  t = new Text(Rect(left, top, width, height));
  t->setCaption("Code:");
  t->setTextSize(1.5f);
  addWidget(t);
  top += height;

  t = new Text(Rect(left, top, width, height));
  t->setCaption("Gregory Shtrasberg aka NetStranger");
  addWidget(t);
  top += height + 0.05;

  t = new Text(Rect(left, top, width, height));
  t->setCaption("Assets used:");
  t->setTextSize(1.5f);
  addWidget(t);
  top += height;

  t = new Text(Rect(left, top, width, height));
  t->setCaption("Box2D physical engine");
  addWidget(t);
  top += height + 0.05;

  t = new Text(Rect(left, top, width, height));
  t->setCaption("Special thanks to:");
  t->setTextSize(1.5f);
  addWidget(t);
  top += height;

  t = new Text(Rect(left, top, width, height));
  t->setCaption("Nika the cat");
  addWidget(t);
  top += height;
}

Credits::~Credits()
{

}
