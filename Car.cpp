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

CarFactory::CarFactory(b2World* world, GenomeSettings* settings):world_(world), settings_(settings)
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
  int actualNumEdges = settings_->getNumEdgeGenes();
  int actualNumWheels = 0;//rand()%(actualNumEdges-2);
  if (actualNumWheels > settings_->getNumWheelGenes()) {
    actualNumWheels = settings_->getNumWheelGenes();
  }
  genome[++gene] = actualNumWheels;
  //genome[gene] = mutateWheel(genome[gene]);
  for (int i=0; i<settings_->getNumWheelGenes(); ++i) {
    genome[++gene] = (rand()%actualNumEdges);
    genome[++gene] = iRand(settings_->getMinWheelRadius(), settings_->getMaxWheelRadius());
    genome[++gene] = iRand(settings_->getMinWheelTorque(), settings_->getMaxWheelTorque());
    genome[++gene] = iRand(settings_->getMinWheelSpeed(), settings_->getMaxWheelSpeed());
  }
  int angle = 0;
  genome[++gene] = actualNumEdges;
  for (int i=settings_->getNumWheelGenes(); i<settings_->getNumWheelGenes()+settings_->getNumEdgeGenes(); ++i) {
    genome[++gene] = angle;
    assert (genome[gene] < 360);
    genome[++gene] = iRand(settings_->getMinEdgeLength(), settings_->getMaxEdgeLength());
    int nextAngle = (i-settings_->getNumWheelGenes()+1)*360/settings_->getNumEdgeGenes();
    angle = iRand(nextAngle-5, min(nextAngle+5, 360));
  }
  assert (gene == settings_->getGenomeSize()-1);
}

void CarFactory::fillCarFromGenome(Car* car)
{
  int gene = 0;
  for (int i=0; i<settings_->getNumWheelGenes(); ++i) {
    car->wheels_[i].vertexId = car->genome_[++gene];
    car->wheels_[i].radius = car->genome_[++gene]*0.01f;
    car->wheels_[i].torque = car->genome_[++gene]*0.01f;
    car->wheels_[i].speed = car->genome_[++gene]*0.01f;
  }
  ++gene;
  for (int i=0; i<settings_->getNumEdgeGenes(); ++i) {
    car->edges_[i].angle = (float)DegToRad(car->genome_[++gene]);
    car->edges_[i].length = car->genome_[++gene]*0.01f;
  }
  assert(gene == settings_->getGenomeSize()-1);
}

Car* CarFactory::cloneCar( Car* car )
{
  Car* newCar = new Car(this);

  cloneGenome(car->genome_, newCar->genome_);
  fillCarFromGenome(newCar);

  createGeneratedCar(newCar);
  return newCar;
}

Car* CarFactory::breedCars( Car* car1, Car* car2, float x, float y )
{
  assert (car1 != car2);
  Car* car = new Car(this);
  //car->initialDist_ = (car1->maxDist_ < car2->maxDist_)?car1->body_->GetPosition() : car2->body_->GetPosition();
  car->initialDist_ += b2Vec2(x, y + 1.0f);
  int gene = -1;

  car->genome_[++gene] = breedWheelGene(car1->genome_[gene], car2->genome_[gene]);
  for (int i=0; i<settings_->getNumWheelGenes(); ++i) {
    car->genome_[++gene] = breedGene(car1->genome_[gene], car2->genome_[gene], 0, settings_->getNumEdgeGenes()-1);
    car->genome_[++gene] = breedGene(car1->genome_[gene], car2->genome_[gene], settings_->getMinWheelRadius(), settings_->getMaxWheelRadius());
    car->genome_[++gene] = breedGene(car1->genome_[gene], car2->genome_[gene], settings_->getMinWheelTorque(), settings_->getMaxWheelTorque());
    car->genome_[++gene] = breedGene(car1->genome_[gene], car2->genome_[gene], settings_->getMinWheelSpeed(), settings_->getMaxWheelSpeed());
  }

  car->genome_[++gene] = car2->genome_[gene];
  for (int i=0; i<settings_->getNumEdgeGenes(); ++i) {
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
    car->genome_[++gene] = breedGene(car1->genome_[gene], car2->genome_[gene], settings_->getMinEdgeLength(), settings_->getMaxEdgeLength());
  }
  assert (gene == settings_->getGenomeSize()-1);
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
  for (int i=0; i<settings_->getNumWheelGenes(); ++i) {
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
  
  for (int i=0; i<settings_->getNumEdgeGenes(); ++i) {
    int nextIdx = i+1;
    if (nextIdx >= car->genome_[settings_->getNumEdgesPosition()]) {
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
  for (int i=0; i<settings_->getNumWheelGenes(); ++i) {
    int wheelExists = car->genome_[settings_->getNumWheelsPosition()] & wheelMask;
    wheelMask <<= 1;
    Wheel& wheel = car->wheels_[i];
    if (!wheelExists || wheel.vertexId >= car->genome_[settings_->getNumEdgesPosition()]) {
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
  for (int i=0; i<settings_->getGenomeSize(); ++i) {
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
  if (canMutate && rand()%100 < settings_->getMutationRate()) {
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
  if (rand()%100 < settings_->getMutationRate()) {
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

  return res & settings_->getAllWheelsMask();
}

int CarFactory::mutateWheel( int wheelGene )
{
  int newWheel = (1 << rand()%settings_->getNumWheelGenes());
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
    for (int i=0; i<fact_->getSettings()->getNumWheelGenes(); ++i) {
      if (!wheels_[i].body) {
        continue;
      }
      wheels_[i].body->SetActive(false);
    }
    return true;
  }

  for (int i=0; i<fact_->getSettings()->getNumWheelGenes(); ++i) {
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

GenomeSettings::GenomeSettings()
{

}

GenomeSettings::~GenomeSettings()
{

}

bool GenomeSettings::init(int minEdgeLength, int maxEdgeLength, int minWheelSpeed, int maxWheelSpeed, int MinWheelTorque, int maxWheelTorque, int numWheelGenes, int numEdgeGenes, int mutationRate)
{
  mutationRate_ = mutationRate;
  minEdgeLength_ = minEdgeLength;
  maxEdgeLength_ = maxEdgeLength;
  minWheelSpeed_ = minWheelSpeed;
  maxWheelSpeed_ = maxWheelSpeed;
  minWheelTorque_ = MinWheelTorque;
  maxWheelTorque_ = maxWheelTorque;
  numWheelGenes_ = numWheelGenes;
  numEdgeGenes_ = numEdgeGenes;
  assert (numEdgeGenes_ <= MAX_EDGE_GENES && numWheelGenes_ <= MAX_WHEEL_GENES);

  minWheelRadius_ = minEdgeLength_/3;
  maxWheelRadius_ = maxEdgeLength_/2;

  allWheelsMask_ = (1 << numWheelGenes_)-1;
  numWheelsPosition_ = 0;
  numEdgesPosition_ = WHEEL_GENE_SIZE*numWheelGenes_+1;
  genomeSize_ = WHEEL_GENE_SIZE*numWheelGenes_ + EDGE_GENE_SIZE*numEdgeGenes_ + 2;
  assert (genomeSize_ <= MAX_GENOME_SIZE);
  return true;
}
