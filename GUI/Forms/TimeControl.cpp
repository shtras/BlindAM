#include "StdAfx.h"
#include "TimeControl.h"
#include "BoxCar.h"

TimeControl::TimeControl( Rect size ):Widget(size)
{
  pauseButton_ = new Button(Rect(0.0, 0.0, 0.33, 1.0));
  pauseButton_->setCaption("||");
  pauseButton_->sigClick.connect(this, &TimeControl::pauseButtonClick);
  addWidget(pauseButton_);
  pauseButton_->setHighlighted(BoxCar::getInstance().isPaused());

  Button* speedDownButton = new Button(Rect(0.33, 0.0, 0.33, 1.0));
  speedDownButton->setCaption("-");
  speedDownButton->sigClick.connect(&BoxCar::getInstance(), &BoxCar::speedDown);
  addWidget(speedDownButton);

  Button* speedUpButton = new Button(Rect(0.66, 0.0, 0.33, 1.0));
  speedUpButton->setCaption("+");
  speedUpButton->sigClick.connect(&BoxCar::getInstance(), &BoxCar::speedUp);
  addWidget(speedUpButton);
}

TimeControl::~TimeControl()
{

}

void TimeControl::pauseButtonClick()
{
  BoxCar::getInstance().togglePause();
  pauseButton_->setHighlighted(BoxCar::getInstance().isPaused());
}
