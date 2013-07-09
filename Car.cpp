#include "StdAfx.h"
#include "Car.h"

float fRand(float from, float to)
{
  return (rand()%100) * (to - from)/100.0f + from;
}

int iRand(int from, int to)
{
  return from + rand()%(to-from);
}

CarFactory::CarFactory(b2World* world):world_(world)
{

}

CarFactory::~CarFactory()
{

}

Car* CarFactory::generateCar()
{
  Car* car = new Car(this);
  generateRandomGenome(car->genome_);
  fillCarFromGenome(car);
  createGeneratedCar(car);
  return car;
}

void CarFactory::generateRandomGenome( int* genome )
{
  int gene = -1;
  int actualNumEdges = NUM_EDGE_GENES;
  int actualNumWheels = 0;//rand()%(actualNumEdges-2);
  if (actualNumWheels > NUM_WHEEL_GENES) {
    actualNumWheels = NUM_WHEEL_GENES;
  }
  genome[++gene] = actualNumWheels;
  //genome[gene] = mutateWheel(genome[gene]);
  for (int i=0; i<NUM_WHEEL_GENES; ++i) {
    genome[++gene] = (rand()%actualNumEdges);
    genome[++gene] = iRand(MIN_WHEEL_RADIUS, MAX_WHEEL_RADIUS);
    genome[++gene] = iRand(MIN_WHEEL_TORQUE, MAX_WHEEL_TORQUE);
    genome[++gene] = iRand(MIN_WHEEL_SPEED, MAX_WHEEL_SPEED);
  }
  int angle = 0;
  genome[++gene] = actualNumEdges;
  for (int i=NUM_WHEEL_GENES; i<NUM_WHEEL_GENES+NUM_EDGE_GENES; ++i) {
    genome[++gene] = angle;
    assert (genome[gene] < 360);
    genome[++gene] = iRand(MIN_EDGE_LENGTH, MAX_EDGE_LENGTH);
    int nextAngle = (i-NUM_WHEEL_GENES+1)*360/NUM_EDGE_GENES;
    angle = iRand(nextAngle-5, min(nextAngle+5, 360));
  }
  assert (gene == GENOME_SIZE-1);
}

void CarFactory::fillCarFromGenome(Car* car)
{
  int gene = 0;
  for (int i=0; i<NUM_WHEEL_GENES; ++i) {
    car->wheels_[i].vertexId = car->genome_[++gene];
    car->wheels_[i].radius = car->genome_[++gene]*0.01f;
    car->wheels_[i].torque = car->genome_[++gene]*0.01f;
    car->wheels_[i].speed = car->genome_[++gene]*0.01f;
  }
  ++gene;
  for (int i=0; i<NUM_EDGE_GENES; ++i) {
    car->edges_[i].angle = (float)DegToRad(car->genome_[++gene]);
    car->edges_[i].length = car->genome_[++gene]*0.01f;
  }
  assert(gene == GENOME_SIZE-1);
}

Car* CarFactory::cloneCar( Car* car )
{
  Car* newCar = new Car(this);

  cloneGenome(car->genome_, newCar->genome_);
  fillCarFromGenome(newCar);

  createGeneratedCar(newCar);
  return newCar;
}

Car* CarFactory::breedCars( Car* car1, Car* car2, float minDist )
{
  assert (car1 != car2);
  Car* car = new Car(this);
  //car->initialDist_ = (car1->maxDist_ < car2->maxDist_)?car1->body_->GetPosition() : car2->body_->GetPosition();
  car->initialDist_ += b2Vec2(minDist,1.0f);
  int gene = -1;

  car->genome_[++gene] = breedWheelGene(car1->genome_[gene], car2->genome_[gene]);
  for (int i=0; i<NUM_WHEEL_GENES; ++i) {
    car->genome_[++gene] = breedGene(car1->genome_[gene], car2->genome_[gene], 0, NUM_EDGE_GENES-1);
    car->genome_[++gene] = breedGene(car1->genome_[gene], car2->genome_[gene], MIN_WHEEL_RADIUS, MAX_WHEEL_RADIUS);
    car->genome_[++gene] = breedGene(car1->genome_[gene], car2->genome_[gene], MIN_WHEEL_TORQUE, MAX_WHEEL_TORQUE);
    car->genome_[++gene] = breedGene(car1->genome_[gene], car2->genome_[gene], MIN_WHEEL_SPEED, MAX_WHEEL_SPEED);
  }

  car->genome_[++gene] = car2->genome_[gene];
  for (int i=0; i<NUM_EDGE_GENES; ++i) {
    car->genome_[++gene] = breedGene(car1->genome_[gene], car2->genome_[gene], 0, 355, false);
    if (i > 0) {
      if (car->genome_[gene] <= car->genome_[gene-EDGE_GENE_SIZE]) {
        car->genome_[gene] = car->genome_[gene-EDGE_GENE_SIZE] + 5;
      }
      if (car->genome_[gene] - car->genome_[gene - EDGE_GENE_SIZE] >= 180) {
        car->genome_[gene] = car->genome_[gene - EDGE_GENE_SIZE] + 175;
      }
    }
    assert (car->genome_[gene] < 360);
    car->genome_[++gene] = breedGene(car1->genome_[gene], car2->genome_[gene], MIN_EDGE_LENGTH, MAX_EDGE_LENGTH);
  }
  assert (gene == GENOME_SIZE-1);
  //cloneGenome(car1->genome_, car->genome_);
  fillCarFromGenome(car);
  createGeneratedCar(car);
  return car;
}

void CarFactory::destroyCar( Car* car )
{
  if (car->destroyed_) {
    return;
  }
  world_->DestroyBody(car->body_);
  for (int i=0; i<NUM_WHEEL_GENES; ++i) {
    if (!car->wheels_[i].body) {
      continue;
    }
    world_->DestroyBody(car->wheels_[i].body);
  }
  car->destroyed_ = true;
}

void CarFactory::createGeneratedCar( Car* car )
{
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  car->body_ = world_->CreateBody(&bodyDef);
  
  for (int i=0; i<NUM_EDGE_GENES; ++i) {
    int nextIdx = i+1;
    if (nextIdx >= car->genome_[NUM_EDGES_POSITION]) {
      nextIdx = 0;
    }
    b2Vec2 verts[3];
    verts[0] = getVecFromLengthAngle(car->edges_[i].length, car->edges_[i].angle);
    verts[1] = getVecFromLengthAngle(car->edges_[nextIdx].length, car->edges_[nextIdx].angle);
    verts[2] = b2Vec2(0, 0);
    if (car->edges_[nextIdx].angle - car->edges_[i].angle > 180 && nextIdx == 0) {
      break;
    }
    b2PolygonShape shape;
    shape.Set(verts, 3);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.filter.categoryBits = 1;
    fixtureDef.filter.maskBits = 2;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.9f;
    car->body_->CreateFixture(&fixtureDef);
    if (nextIdx == 0) {
      break;
    }
  }
  car->body_->SetTransform(car->initialDist_, 0);

  int wheelMask = 1;
  for (int i=0; i<NUM_WHEEL_GENES; ++i) {
    int wheelExists = car->genome_[NUM_WHEELS_POSITION] & wheelMask;
    wheelMask <<= 1;
    Wheel& wheel = car->wheels_[i];
    if (!wheelExists || wheel.vertexId >= car->genome_[NUM_EDGES_POSITION]) {
      wheel.body = NULL;
      wheel.joint = NULL;
      continue;
    }
    b2CircleShape circle;
    circle.m_radius = wheel.radius;
    b2FixtureDef fd;
    fd.shape = &circle;
    fd.density = 1.0f;
    fd.friction = 0.7f;
    fd.filter.categoryBits = 1;
    fd.filter.maskBits = 2;
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    b2Vec2 wheelVertex = getVecFromLengthAngle(car->edges_[wheel.vertexId].length, car->edges_[wheel.vertexId].angle);
    bd.position.Set(wheelVertex.x, wheelVertex.y);
    wheel.body = world_->CreateBody(&bd);
    wheel.body->CreateFixture(&fd);

    wheel.body->SetTransform(car->initialDist_ + wheelVertex, 0);

    b2WheelJointDef jd;
    b2Vec2 axis(0.0f, 1.0f);

    jd.Initialize(car->body_, wheel.body, wheel.body->GetPosition(), axis);
    jd.motorSpeed = -wheel.speed;
    jd.maxMotorTorque = wheel.torque;
    jd.enableMotor = true;
    jd.frequencyHz = 4.0f;
    jd.dampingRatio = 0.7f;
    wheel.joint = (b2WheelJoint*)world_->CreateJoint(&jd);
    ++car->numWheels_;
  }
}

Car* CarFactory::getOneOfCars( Car* car1, Car* car2 )
{
  if (rand()%2) {
    return car1;
  }
  return car2;
}

Car* CarFactory::getOtherCar( Car* car, Car* car1, Car* car2 )
{
  if (car == car1) {
    return car2;
  }
  assert (car == car2);
  return car1;
}

b2Vec2 CarFactory::getVecFromLengthAngle( float length, float angle )
{
  return b2Vec2(length*cos(angle), length*sin(angle));
}

void CarFactory::cloneGenome( int* from, int* to )
{
  for (int i=0; i<GENOME_SIZE; ++i) {
    to[i] = from[i];
  }
}

int CarFactory::breedGene( int g1, int g2, int minV, int maxV, bool canMutate/*=true*/ )
{
  if (g1 < 0 || g2 < 0) {
    return -1;
  }
  int res = 0;
  if (rand()%2) {
    res = g1;
  } else {
    res = g2;
  }
  if (canMutate && rand()%100 < 5) {
    res = iRand(minV, maxV);
  }
  if (res < minV) {
    res = minV;
  }
  if (res > maxV) {
    res = maxV;
  }
  return res;
}

int CarFactory::breedWheelGene(int g1, int g2)
{
  if (g1 < 0 || g2 < 0) {
    return -1;
  }
  int res = 0;
  if (rand()%2) {
    res = g1;
  } else {
    res = g2;
  }
  if (rand()%100 < 5) {
    switch (rand()%3)
    {
    case 0:
      res = mutateWheel(res);
      break;
    case 1:
      res >>= 1;
      break;
    case 2:
      res <<= 1;
      break;
    default:
      break;
    }
  }

  return res & ALL_WHEELS_MASK;
}

int CarFactory::mutateWheel( int wheelGene )
{
  int newWheel = (1 << rand()%NUM_WHEEL_GENES);
  return wheelGene ^ newWheel;
}

Car::Car(CarFactory* fact):currDist_(0), maxDist_(0), idleCounter_(0), dead_(false), destroyed_(false), numWheels_(0), fact_(fact), detachedWheelSteps_(0),
  initialDist_(b2Vec2(0,0))
{

}

Car::~Car()
{

}

bool Car::isActive()
{
  return !dead_;
}

bool Car::step()
{
  if (dead_) {
    return false;
  }
  currDist_ = body_->GetPosition().x;
  if (currDist_ > lastDist_) {
    --idleCounter_;
  }
  if (currDist_ - maxDist_ > 0.03f) {
    maxDist_ = currDist_;
    --idleCounter_;
  } else {
    idleCounter_ += 2;
  }
  if (idleCounter_ < 0) {
    idleCounter_ = 0;
  }
  if (idleCounter_ >= IDLE_STEPS_TO_DIE) {
    dead_ = true;
    body_->SetActive(false);
    for (int i=0; i<NUM_WHEEL_GENES; ++i) {
      if (!wheels_[i].body) {
        continue;
      }
      wheels_[i].body->SetActive(false);
    }
    return true;
  }

  for (int i=0; i<NUM_WHEEL_GENES; ++i) {
    if (wheels_[i].body && !wheels_[i].joint) {
      ++detachedWheelSteps_;
      if (detachedWheelSteps_ > 100) {
        fact_->destroyBody(wheels_[i].body);
        wheels_[i].body = NULL;
      }
    }
    if (!wheels_[i].body || !wheels_[i].joint) {
      continue;
    }
    assert(wheels_[i].joint);
    if (wheels_[i].joint->GetReactionForce(1.0f).Length() > 9.0f) {
      //cout << wheels_[i].joint->GetReactionForce(1.0f).Length() << endl;
      fact_->destroyJoint(wheels_[i].joint);
      wheels_[i].joint = NULL;
    }
    
  }
  lastDist_ = currDist_;
  return false;
}
