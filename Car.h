#pragma once
#include "Box2D/Box2D.h"

#define IDLE_STEPS_TO_DIE 600

// #define MIN_EDGE_LENGTH 30
// #define MAX_EDGE_LENGTH 200
// 
// #define MIN_WHEEL_RADIUS (MIN_EDGE_LENGTH/3)
// #define MAX_WHEEL_RADIUS (MAX_EDGE_LENGTH/2)
// 
// #define MIN_WHEEL_SPEED 100
// #define MAX_WHEEL_SPEED 2000
// 
// #define MIN_WHEEL_TORQUE 100
// #define MAX_WHEEL_TORQUE 1000
// 
#define WHEEL_GENE_SIZE 4 //vertex id, radius, torque, speed
// #define NUM_WHEEL_GENES 4
// #define ALL_WHEELS_MASK ((1<<(NUM_WHEEL_GENES))-1)
#define EDGE_GENE_SIZE 2 //angle, length
// #define NUM_EDGE_GENES 8
// #define GENOME_SIZE ((((WHEEL_GENE_SIZE)*(NUM_WHEEL_GENES)+((EDGE_GENE_SIZE)*NUM_EDGE_GENES)))+2)
// //numWheels, [vertex id, radius, torque]*, numEdges, [angle, length]*
// 
// #define NUM_EDGES_POSITION ((NUM_WHEEL_GENES)*(WHEEL_GENE_SIZE)+1)
// #define NUM_WHEELS_POSITION 0
#define MAX_EDGE_GENES 16
#define MAX_WHEEL_GENES MAX_EDGE_GENES
#define MAX_GENOME_SIZE ((WHEEL_GENE_SIZE)*(MAX_WHEEL_GENES) + (EDGE_GENE_SIZE)*(MAX_EDGE_GENES) + 2)

float fRand(float from, float to);

class Car;

class GenomeSettings
{
public:
  GenomeSettings();
  ~GenomeSettings();
  bool init(int minEdgeLength, int maxEdgeLength, int minWheelSpeed, int maxWheelSpeed, int MinWheelTorque, int maxWheelTorque, int numWheelGenes, int numEdgeGenes, int mutationRate);
  int getMinEdgeLength() const {return minEdgeLength_;}
  int getMaxEdgeLength() const {return maxEdgeLength_;}
  int getMinWheelRadius() const {return minWheelRadius_;}
  int getMaxWheelRadius() const {return maxWheelRadius_;}
  int getMinWheelSpeed() const {return minWheelSpeed_;}
  int getMaxWheelSpeed() const {return maxWheelSpeed_;}
  int getMinWheelTorque() const {return minWheelTorque_;}
  int getMaxWheelTorque() const {return maxWheelTorque_;}
  int getNumWheelGenes() const {return numWheelGenes_;}
  int getNumEdgeGenes() const {return numEdgeGenes_;}
  int getAllWheelsMask() const {return allWheelsMask_;}
  int getGenomeSize() const {return genomeSize_;}
  int getNumEdgesPosition() const {return numEdgesPosition_;}
  int getNumWheelsPosition() const {return numWheelsPosition_;}
  int getMutationRate() const {return mutationRate_;}
private:
  int minEdgeLength_;
  int maxEdgeLength_;
  int minWheelRadius_;
  int maxWheelRadius_;
  int minWheelSpeed_;
  int maxWheelSpeed_;
  int minWheelTorque_;
  int maxWheelTorque_;
  int numWheelGenes_;
  int numEdgeGenes_;
  int allWheelsMask_;
  int genomeSize_;
  int numEdgesPosition_;
  int numWheelsPosition_;
  int mutationRate_;
};

struct Edge
{
  float length;
  float angle;
};

struct Wheel
{
  int vertexId;
  float radius;
  float torque;
  float speed;
  b2Body* body;
  b2Joint* joint;
};

class CarFactory
{
public:
  CarFactory(b2World* world, GenomeSettings* settings);
  ~CarFactory();
  Car* generateCar();
  Car* breedCars(Car* car1, Car* car2, float x, float y);
  Car* cloneCar(Car* car);
  void destroyCar(Car* car);
  void destroyJoint(b2Joint* joint) {world_->DestroyJoint(joint);}
  void destroyBody(b2Body* body) {world_->DestroyBody(body);}
  const GenomeSettings* getSettings() const {return settings_;}
private:
  Car* getOneOfCars(Car* car1, Car* car2);
  Car* getOtherCar(Car* car, Car* car1, Car* car2);
  void fillCarFromGenome(Car* car);
  void generateRandomGenome(int* genome);
  void createGeneratedCar(Car* car);
  b2Vec2 getVecFromLengthAngle(float length, float angle);
  void cloneGenome(int* from, int* to);
  int breedGene(int g1, int g2, int minV, int maxV, bool canMutate=true);
  int breedWheelGene(int g1, int g2);
  int mutateWheel(int wheelGene);
  b2World* world_;
  GenomeSettings* settings_;
};

class Car
{
  friend class CarFactory;
public:
  ~Car();
  const b2Body* getBody() const {return body_;}
  bool step();
  bool isActive();
  float getMaxDist() {return maxDist_;}
  bool isDead() {return dead_;}
  int numWheels() {return numWheels_;}
  float getDist() {return currDist_;}
private:
  Car(CarFactory* fact);
  b2Body* body_;

  Wheel wheels_[MAX_WHEEL_GENES];
  Edge edges_[MAX_EDGE_GENES];

  float maxDist_;
  float currDist_;
  float lastDist_;
  int idleCounter_;
  bool dead_;
  bool destroyed_;
  int numWheels_;
  int genome_[MAX_GENOME_SIZE];
  int detachedWheelSteps_;
  CarFactory* fact_;
  b2Vec2 initialDist_;
};
