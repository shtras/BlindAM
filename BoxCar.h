#pragma once
#include "World.h"
#include "Input.h"

class BoxCar: public has_slots<>
{
public:
  enum GameState {Menu, Game, ShowCredits, Quit};
  static BoxCar& getInstance();
  bool init();
  bool run();
  void toggleMenu();
  void toggleGame();
  void toggleCredits();
  void togglePause();
  bool isPaused() {return paused_;}
private:
  bool mainLoop();
  bool initMenu();
  bool finishMenu();
  bool initGame();
  bool finishGame();
  bool initCredits();
  bool finishCredits();
  void handleEvent(SDL_Event& event);
  void quit();
  void close();
  void birthModeClick(void* param);
  BoxCar();
  ~BoxCar();
  GameState state_;
  bool stateRunnig_;
  bool paused_;
  World* world_;
  int speed_;

  Input* seedInput_;
  Input* popSizeInput_;
  Input* breedThresholdInput_;

  Input* minEdgeLengthInput_;
  Input* maxEdgeLengthInput_;
  Input* minWheelSpeedInput_;
  Input* maxWheelSpeedInput_;
  Input* minWheelTorqueInput_;
  Input* maxWheelTorqueInput_;
  Input* numWheelsInput_;
  Input* numEdgesInput_;
  Input* mutationRateInput_;
  int seed_;
  CString seedStr_;
  int popSize_;
  int breedThreshold_;
  GenomeSettings* settings_;
  Button* birthModeButtons_[10];
  World::BirthPosition birthMode_;
};
