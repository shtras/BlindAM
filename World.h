#pragma once
#include "Box2D/Box2D.h"
#include "GlobalWindow.h"
#include "MyB2Draw.h"
#include "Car.h"
#include "TextArea.h"
#include "Text.h"
#include "Graph.h"
#include "Button.h"

class World: public GlobalWindow
{
public:
  enum BirthPosition {TrackStart=0, MinBestDist, LastMode};
  World(Rect size, int seed, int popSize, int breedThreshold, GenomeSettings* settings, BirthPosition mode);
  ~World();
  void step();
  void render();

  void onMouseMove();
  void onMouseWheelScroll(int direction);
  void onRMUp();
  void onRMDown();
private:
  void createPath();
  void performBreed();
  void killAndBreed( int killIdx, Car* car2, Car* car3 );
  void followLeaderClick();
  b2World* world_;
  MyB2Draw* draw_;
  vector<Car*> cars_;
  CarFactory* fact_;
  uint32_t populationSize_;
  FramedText* info_;
  Button* followLeaderButton_;
  Graph* distanceGraph_;
  Graph* wheelGraph_;
  int breedCount_;
  int breedThreshold_;
  int graphCount_;
  int graphThreshold_;
  vector<int> deadIndeces_;
  int* wheelCount_;
  float highScore_;
  float minDist_;
  vector<float> heightMap_;
  BirthPosition birthPos_;
  GenomeSettings* settings_;
};

