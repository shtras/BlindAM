#include "StdAfx.h"
#include "World.h"
#include "BoxCar.h"
#include "Button.h"

World::World(Rect size, int seed, int popSize, int breedThreshold): GlobalWindow(size), fact_(NULL), populationSize_(popSize), 
  breedCount_(0), breedThreshold_(breedThreshold), graphCount_(0), graphThreshold_(500), highScore_(0), minDist_(0)
{
  b2Vec2 gravity(0.0f, -5.0f);
  world_ = new b2World(gravity);
  srand(seed);
  draw_ = new MyB2Draw();
  draw_->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit);

  world_->SetDebugDraw(draw_);
  fact_ = new CarFactory(world_);

  float right = -50.0f;
  float top = -3.0f;
  float angle = 0.0f;
  float halfWidth = 2.5f;
  for (int i=0; i<500; ++i) {
    b2BodyDef bodyDef1;
    bodyDef1.position.Set(right, top);
    b2Body* body1 = world_->CreateBody(&bodyDef1);
    b2PolygonShape box1;
    box1.SetAsBox(halfWidth, 0.2f);

    b2FixtureDef gfd;
    gfd.shape = &box1;
    gfd.density = 1.0f;
    gfd.friction = 10.0f;
    gfd.filter.categoryBits = 2;
    gfd.filter.maskBits = 1;

    body1->CreateFixture(&gfd);
    int step = (int)(30*pow(i*0.05f+1, 0.6));
    angle = 0.02f*(rand()%step - step/2);
    body1->SetTransform(body1->GetPosition() + b2Vec2(halfWidth*cos(angle), halfWidth*sin(angle)), angle);

    right += halfWidth*2.0f * cos(angle);
    top += halfWidth*2.0f * sin(angle);
  }

  //////////////////////////////////////////////////////////////////////////

  for(uint32_t i=0; i<populationSize_; ++i) {
    cars_.push_back(fact_->generateCar());
  }
  
  Button* quitButton = new Button(Rect(0.9, 0.0, 0.1, 0.05));
  quitButton->setCaption("Quit");
  quitButton->sigClick.connect(&BoxCar::getInstance(), &BoxCar::toggleMenu);
  addWidget(quitButton);

  info_ = new FramedText(Rect(0.3, 0.0, 0.5, 0.05));
  addWidget(info_);

  distanceGraph_ = new Graph(Rect(0.0, 0.0, 0.3, 0.3), 50000, 2);
  addWidget(distanceGraph_);
  distanceGraph_->setColor(0, Vector3(255,0,100));
  distanceGraph_->setColor(1, Vector3(255,255,0));

  distanceGraph_->addValue(0,0);
  distanceGraph_->addValue(1,0);

  wheelGraph_ = new Graph(Rect(0.0, 0.3, 0.3, 0.3), 50000, NUM_WHEEL_GENES);
  addWidget(wheelGraph_);
  wheelGraph_->setColor(0, Vector3(255,0,0));
  if (NUM_WHEEL_GENES > 1) {
    wheelGraph_->setColor(1, Vector3(0,255,0));
  }
  if (NUM_WHEEL_GENES > 2) {
    wheelGraph_->setColor(2, Vector3(255,50,200));
  }
  if (NUM_WHEEL_GENES > 3) {
    wheelGraph_->setColor(3, Vector3(20,150,100));
  }
  if (NUM_WHEEL_GENES > 4) {
    wheelGraph_->setColor(4, Vector3(0,255,200));
  }
  if (NUM_WHEEL_GENES > 5) {
    wheelGraph_->setColor(5, Vector3(0,0,255));
  }

  wheelGraph_->addValue(0, (float)populationSize_);
  for (int i=1; i<NUM_WHEEL_GENES; ++i) {
    wheelGraph_->addValue(i, 0);
  }

  followLeaderButton_ = new Button(Rect(0.8,0.0,0.1,0.05));
  followLeaderButton_->setCaption("Follow leader");
  followLeaderButton_->sigClick.connect(this, &World::followLeaderClick);
  addWidget(followLeaderButton_);
}

World::~World()
{
  delete fact_;
  delete world_;
}

void World::step()
{
  int alive = 0;
  world_->Step(1.0f/60.0f, 6, 2);
  float maxDist = -1000.0f;
  Car* leader = NULL;
  float average = 0;
  for (int i=0; i<NUM_WHEEL_GENES; ++i) {
    wheelCount_[i] = 0;
  }
  int SZ = cars_.size();
  minDist_ = 100000.0f;
  for (int i=0; i<SZ; ++i) {
    Car* car = cars_[i];
    bool wasDead = car->isDead();
    if (car->step()) {
      //Died in this step. The most intuitive return value EVAR
      deadIndeces_.push_back(i);
      assert (!wasDead && car->isDead());
      assert (deadIndeces_.size() <= populationSize_);
    }
    if (car->isActive() && car->getBody()->GetPosition().x > maxDist) {
      maxDist = car->getBody()->GetPosition().x;
      leader = car;
    }
    if (car->getBody()->GetPosition().x > highScore_) {
      highScore_ = car->getBody()->GetPosition().x;
    }
    if (car->isActive()) {
      ++alive;
    } else {
      //fact_->destroyCar(car);
    }
    assert (car->numWheels() < NUM_WHEEL_GENES);
    ++wheelCount_[car->numWheels()];
    average += car->getMaxDist();
    if (car->getDist() < minDist_) {
      minDist_ = car->getDist();
    }
  }

  average /= (float)populationSize_;
  if (++breedCount_ > breedThreshold_) {
    performBreed();
    breedCount_ = 0;
  }
  if (++graphCount_ > graphThreshold_) {
    graphCount_ = 0;
    distanceGraph_->addValue(0, average);
    distanceGraph_->addValue(1, highScore_);
    for (int i=0; i<NUM_WHEEL_GENES; ++i) {
      wheelGraph_->addValue(i, (float)wheelCount_[i]);
    }
  }
  if (leader) {
    info_->setCaption("Score: " + CString(leader->getMaxDist(), 2) + " High: " + CString(highScore_, 2) + " Alive: " + CString(alive) + "(" + CString((int)populationSize_) + ") Average: " + CString(average, 2));
    if (followLeaderButton_->isHighlighted()) {
      draw_->setOffset(leader->getBody()->GetPosition().x, leader->getBody()->GetPosition().y);
    }
  }
}

void World::render()
{
  draw_->scrollZoomStep();
  world_->DrawDebugData();
}

void World::onMouseMove()
{
  draw_->scrollMMove();
}

void World::onMouseWheelScroll( int direction )
{
  draw_->scrollMScroll(direction);
}

void World::onRMUp()
{
  draw_->scrollMUp();
}

void World::onRMDown()
{
  draw_->scrollMDown();
}

bool compareCars(Car* c1, Car* c2)
{
  if (c1->getMaxDist() > c2->getMaxDist()) {
    return true;
  }
  return false;
}

void World::startNewGeneration()
{
  //Deprecated function. Was in use before the "blind watchman" algorithm
  sort (cars_.begin(), cars_.end(), compareCars);  
  vector<Car*> newCars;
  newCars.push_back(fact_->cloneCar(cars_[0]));
  int fracture = populationSize_/4;
  int starter = 1;
  while (newCars.size() < populationSize_) {
    for (uint32_t i=0; i<populationSize_/fracture; ++i) {
      if (starter-1 == i) {
        continue;
      }
      newCars.push_back(fact_->breedCars(cars_[starter-1], cars_[i], minDist_));
      if (newCars.size() >= populationSize_) {
        break;
      }
    }
    ++starter;
  }

  for (Car* car: cars_) {
    delete car;
  }
  cars_.clear();

  cars_ = newCars;
}

void World::performBreed()
{
  uint32_t deadSZ = deadIndeces_.size();
  if (deadSZ < max(populationSize_/6, 3)) {
    return;
  }
  int idx1 = rand()%deadSZ;
  int idx2 = idx1;
  int idx3 = idx1;
  while (idx2 == idx1) {
    idx2 = rand()%deadSZ;
  }
  while (idx3 == idx1 || idx3 == idx2) {
    idx3 = rand()%deadSZ;
  }
  int carIdx1 = deadIndeces_[idx1];
  int carIdx2 = deadIndeces_[idx2];
  int carIdx3 = deadIndeces_[idx3];
  Car* car1 = cars_[carIdx1];
  Car* car2 = cars_[carIdx2];
  Car* car3 = cars_[carIdx3];
  assert (car1 != car2 && car2 != car3 && car1 != car2);
  if (car1->getMaxDist() <= car2->getMaxDist() && car1->getMaxDist() <= car3->getMaxDist()) {
    deadIndeces_.erase(deadIndeces_.begin() + idx1);
    killAndBreed(carIdx1, car2, car3);
  } else if (car2->getMaxDist() <= car1->getMaxDist() && car2->getMaxDist() <= car3->getMaxDist()) {
    deadIndeces_.erase(deadIndeces_.begin() + idx2);
    killAndBreed(carIdx2, car1, car3);
  } else if (car3->getMaxDist() <= car1->getMaxDist() && car3->getMaxDist() <= car2->getMaxDist()) {
    deadIndeces_.erase(deadIndeces_.begin() + idx3);
    killAndBreed(carIdx3, car1, car2);
  }
}

void World::killAndBreed( int killIdx, Car* breed1, Car* breed2 )
{
  fact_->destroyCar(cars_[killIdx]);
  delete cars_[killIdx];
  cars_[killIdx] = fact_->breedCars(breed1, breed2, 0);
}

void World::followLeaderClick()
{
  followLeaderButton_->setHighlighted(!followLeaderButton_->isHighlighted());
}
