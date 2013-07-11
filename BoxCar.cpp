#include "StdAfx.h"
#include "BoxCar.h"
#include "Renderer.h"
#include "Button.h"
#include "Credits.h"

#include "Box2D/Box2D.h"

const char* Version = "0.2";

CString getBirthModeName(World::BirthPosition mode)
{
  switch (mode)
  {
  case World::TrackStart:
    return "Track start";
  case World::MinBestDist:
    return "Looser parent";
  default:
    return "Unknown mode";
  }
}

BoxCar::BoxCar():state_(Menu),stateRunnig_(false),paused_(true), world_(NULL), speed_(1), seedStr_("habrahabr.ru"), popSize_(100), breedThreshold_(10), birthMode_(World::TrackStart)
{
#ifdef DEBUG
  popSize_ = 20;
#endif
}

BoxCar::~BoxCar()
{
  delete settings_;
}

BoxCar& BoxCar::getInstance()
{
  static BoxCar instance;
  return instance;
}

bool BoxCar::init()
{
  bool res = Renderer::getInstance().init();
  if (!res) {
    Logger::getInstance().log(ERROR_LOG_NAME, "Failed to initialize renderer");
    return false;
  }

  settings_ = new GenomeSettings();
  settings_->init(30, 200, 100, 2000, 100, 1000, 5, 8, 5);

  return true;
}

bool BoxCar::run()
{
  while (state_ != Quit) {
    switch (state_)
    {
    case Menu:
      Logger::getInstance().log(INFO_LOG_NAME, "Switching to menu");
      initMenu();
      mainLoop();
      finishMenu();
      break;
    case Game:
      Logger::getInstance().log(INFO_LOG_NAME, "Switching to game");
      initGame();
      mainLoop();
      finishGame();
      break;
    case ShowCredits:
      Logger::getInstance().log(INFO_LOG_NAME, "Switching to credits");
      initCredits();
      mainLoop();
      finishCredits();
      break;
    default:
      Logger::getInstance().log(ERROR_LOG_NAME, "Encountered unknown state. Terminating.");
      state_ = Quit;
      break;
    }
  }
  close();
  return true;
}

bool BoxCar::mainLoop()
{
  //typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
  //PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
  //wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

  //if( wglSwapIntervalEXT ) {
  //  wglSwapIntervalEXT(0);
  //} 

  SDL_WM_SetCaption("Blind Auto Mechanic", NULL);

  int fpsTimeBase = SDL_GetTicks();
  int currTime = fpsTimeBase;
  int lastTime = currTime;
  int accumulator = 0;
  int dt = 10;
  int accumulatorEvents = 0;
  int dtEvents = 6;
  int frames = 0;
  float fps = 0;
  CString version = Version + CString(" ") + CString(BUILD_STR);
  while(stateRunnig_) {
    if (accumulatorEvents > dtEvents) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        handleEvent(event);
      }
      accumulatorEvents -= dtEvents;
    }
    currTime = SDL_GetTicks();
    int delta = currTime - lastTime;
    accumulatorEvents += delta;
    if (!paused_) {
      accumulator += delta;
      if (accumulator >= dt*3) {
        accumulator = dt*3;
      }
      while (accumulator >= dt) {
        accumulator -= dt;
        if (state_ == Game && world_) {
          for (int i=0; i<speed_; ++i) {
            world_->step();
          }
        }
      }
    }
    lastTime = currTime;
    ++frames;
    if (currTime - fpsTimeBase > 100) {
      fps = frames / (float)(currTime - fpsTimeBase) * 1000.0f;
      fpsTimeBase = currTime;
      frames = 0;
    }
    Renderer::getInstance().clear();
    Renderer::getInstance().render();
    Renderer::getInstance().setTextSize(1);
    Renderer::getInstance().setColor(Vector4(255, 255, 255, 255));
    CString fpsString = "FPS: " + CString(fps, 2) + " Speed:" + CString(speed_);
    Renderer::getInstance().renderText(0.0f, 0.0f, fpsString);
    Renderer::getInstance().renderText(1.0f - version.getSize()*Renderer::getInstance().getCharWidth(), 1.0f - Renderer::getInstance().getCharHeight(), version);

    Renderer::getInstance().renderEnd();
    checkReleaseError("OpenGL error during main loop. Something bad happened :(");
    Sleep(0);
  }
  return true;
}

void advanceCoords(double& top, double& right, double width, double height)
{
  right += width + 0.05;
  if (right > 1.0 - width) {
    right = 0.1;
    top += height + 0.01;
  }
}

bool BoxCar::initMenu()
{
  Renderer& renderer = Renderer::getInstance();
  stateRunnig_ = true;
  Window* menuWindow = new Window(Rect(0.2, 0.1, 0.6, 0.8));
  renderer.addWidget(menuWindow);
  double top = 0.05;
  double right = 0.1;
  double height = 0.1;
  double width = 0.15;
  Text* t;
  {
    t = new Text(Rect(right, top, width, height*0.5));
    t->setCaption("Seed");
    menuWindow->addWidget(t);
    seedInput_ = new Input(Rect(right, top+height*0.5, width, height*0.5));
    seedInput_->setCaption(seedStr_);
    menuWindow->addWidget(seedInput_);
    advanceCoords(top, right, width, height);

    t = new Text(Rect(right, top, width, height*0.5));
    t->setCaption("Population size");
    menuWindow->addWidget(t);
    popSizeInput_ = new Input(Rect(right, top+height*0.5, width, height*0.5));
    popSizeInput_->setCaption(CString(popSize_));
    menuWindow->addWidget(popSizeInput_);
    advanceCoords(top, right, width, height);

    t = new Text(Rect(right, top, width, height*0.5));
    t->setCaption("Breed threshold");
    menuWindow->addWidget(t);
    breedThresholdInput_ = new Input(Rect(right, top+height*0.5, width, height*0.5));
    breedThresholdInput_->setCaption(CString(breedThreshold_));
    menuWindow->addWidget(breedThresholdInput_);
    advanceCoords(top, right, width, height);

    t = new Text(Rect(right, top, width, height*0.5));
    t->setCaption("Min edge length");
    menuWindow->addWidget(t);
    minEdgeLengthInput_ = new Input(Rect(right, top+height*0.5, width, height*0.5));
    minEdgeLengthInput_->setCaption(CString(settings_->getMinEdgeLength()));
    menuWindow->addWidget(minEdgeLengthInput_);
    advanceCoords(top, right, width, height);

    t = new Text(Rect(right, top, width, height*0.5));
    t->setCaption("Max edge length");
    menuWindow->addWidget(t);
    maxEdgeLengthInput_ = new Input(Rect(right, top+height*0.5, width, height*0.5));
    maxEdgeLengthInput_->setCaption(CString(settings_->getMaxEdgeLength()));
    menuWindow->addWidget(maxEdgeLengthInput_);
    advanceCoords(top, right, width, height);

    t = new Text(Rect(right, top, width, height*0.5));
    t->setCaption("Min wheel torque");
    menuWindow->addWidget(t);
    minWheelTorqueInput_ = new Input(Rect(right, top+height*0.5, width, height*0.5));
    minWheelTorqueInput_->setCaption(CString(settings_->getMinWheelTorque()));
    menuWindow->addWidget(minWheelTorqueInput_);
    advanceCoords(top, right, width, height);

    t = new Text(Rect(right, top, width, height*0.5));
    t->setCaption("Max wheel torque");
    menuWindow->addWidget(t);
    maxWheelTorqueInput_ = new Input(Rect(right, top+height*0.5, width, height*0.5));
    maxWheelTorqueInput_->setCaption(CString(settings_->getMaxWheelTorque()));
    menuWindow->addWidget(maxWheelTorqueInput_);
    advanceCoords(top, right, width, height);

    t = new Text(Rect(right, top, width, height*0.5));
    t->setCaption("Min wheel speed");
    menuWindow->addWidget(t);
    minWheelSpeedInput_ = new Input(Rect(right, top+height*0.5, width, height*0.5));
    minWheelSpeedInput_->setCaption(CString(settings_->getMinWheelSpeed()));
    menuWindow->addWidget(minWheelSpeedInput_);
    advanceCoords(top, right, width, height);

    t = new Text(Rect(right, top, width, height*0.5));
    t->setCaption("Max wheel speed");
    menuWindow->addWidget(t);
    maxWheelSpeedInput_ = new Input(Rect(right, top+height*0.5, width, height*0.5));
    maxWheelSpeedInput_->setCaption(CString(settings_->getMaxWheelSpeed()));
    menuWindow->addWidget(maxWheelSpeedInput_);
    advanceCoords(top, right, width, height);

    t = new Text(Rect(right, top, width, height*0.5));
    t->setCaption("Max wheels");
    menuWindow->addWidget(t);
    numWheelsInput_ = new Input(Rect(right, top+height*0.5, width, height*0.5));
    numWheelsInput_->setCaption(CString(settings_->getNumWheelGenes()));
    menuWindow->addWidget(numWheelsInput_);
    advanceCoords(top, right, width, height);

    t = new Text(Rect(right, top, width, height*0.5));
    t->setCaption("Num edges");
    menuWindow->addWidget(t);
    numEdgesInput_ = new Input(Rect(right, top+height*0.5, width, height*0.5));
    numEdgesInput_->setCaption(CString(settings_->getNumEdgeGenes()));
    menuWindow->addWidget(numEdgesInput_);
    advanceCoords(top, right, width, height);

    t = new Text(Rect(right, top, width, height*0.5));
    t->setCaption("Mutation rate");
    menuWindow->addWidget(t);
    mutationRateInput_ = new Input(Rect(right, top+height*0.5, width, height*0.5));
    mutationRateInput_->setCaption(CString(settings_->getMutationRate()));
    menuWindow->addWidget(mutationRateInput_);
  }

  right = 0.1;
  top += height+ 0.05;
  height *= 0.5f;
  for (int modeItr = World::TrackStart; modeItr < World::LastMode; ++modeItr) {
    birthModeButtons_[modeItr] = new Button(Rect(right, top, width, height));
    advanceCoords(top, right, width, height);
    birthModeButtons_[modeItr]->setCaption(getBirthModeName((World::BirthPosition)modeItr));
    if (birthMode_ == modeItr) {
      birthModeButtons_[modeItr]->setHighlighted(true);
    }
    birthModeButtons_[modeItr]->setParam((void*)modeItr);
    birthModeButtons_[modeItr]->sig1Click.connect(this, &BoxCar::birthModeClick);
    menuWindow->addWidget(birthModeButtons_[modeItr]);
  }

  right = 0.1;
  top += height+ 0.05;
  height *= 2.0f;
  Button* newGameButton = new Button(Rect(0.1, top, 0.8, height));
  newGameButton->setCaption("Start");
  newGameButton->setColor(Vector4(0, 0, 255, 255));
  newGameButton->setTextSize(3);
  menuWindow->addWidget(newGameButton);
  newGameButton->sigClick.connect(this, &BoxCar::toggleGame);
  top += height + 0.05;

  Button* creditsButton = new Button(Rect(0.1, top, 0.8, height));
  creditsButton->setCaption("Credits");
  creditsButton->setColor(Vector4(0, 0, 255, 255));
  creditsButton->setTextSize(3);
  menuWindow->addWidget(creditsButton);
  creditsButton->sigClick.connect(this, &BoxCar::toggleCredits);
  top += height + 0.05;

  Button* quitButton = new Button(Rect(0.1, top, 0.8, height));
  quitButton->setCaption("Quit");
  quitButton->setColor(Vector4(0, 0, 255, 255));
  quitButton->setTextSize(3);
  menuWindow->addWidget(quitButton);
  quitButton->sigClick.connect(this, &BoxCar::quit);
  top += height + 0.05;

  return true;
}

bool BoxCar::finishMenu()
{
  CString seedStr = seedInput_->getCaption();
  seed_ = 0;
  for (int i=0; i<seedStr.getSize(); ++i) {
    seed_ += seedStr[i];
    if (i != seedStr.getSize()-1) {
      seed_ <<= 1;
    }
  }
  sscanf(popSizeInput_->getCaption(), "%d", &popSize_);
  sscanf(breedThresholdInput_->getCaption(), "%d", &breedThreshold_);
  seedStr_ = seedStr;
  Logger::getInstance().log(INFO_LOG_NAME, "Used seed: " + seedStr);
  settings_->init(parseInt(minEdgeLengthInput_->getCaption()),
    parseInt(maxEdgeLengthInput_->getCaption()),
    parseInt(minWheelSpeedInput_->getCaption()),
    parseInt(maxWheelSpeedInput_->getCaption()),
    parseInt(minWheelTorqueInput_->getCaption()),
    parseInt(maxWheelTorqueInput_->getCaption()),
    parseInt(numWheelsInput_->getCaption()),
    parseInt(numEdgesInput_->getCaption()),
    parseInt(mutationRateInput_->getCaption()));

  Renderer::getInstance().clearWindows();
  return true;
}

bool BoxCar::initGame()
{
  paused_ = true;
  stateRunnig_ = true;

  world_ = new World(Rect(0,0,1,1), seed_, popSize_, breedThreshold_, settings_, birthMode_);
  Renderer::getInstance().addWidget(world_);
  return true;
}

bool BoxCar::finishGame()
{
  Renderer::getInstance().clearWindows();
  world_ = NULL;
  speed_ = 1;
  return true;
}

void BoxCar::handleEvent( SDL_Event& event )
{
  switch (event.type)
  {
  case SDL_QUIT:
    quit();
    break;
  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
  case SDL_MOUSEMOTION:
    Renderer::getInstance().handleGUIEvent(event);
    break;
  case SDL_KEYDOWN:
    Renderer::getInstance().handleKeyboardEvent(event);
    if (event.key.keysym.sym == SDLK_SPACE) {
      togglePause();
    }
    if (Renderer::getInstance().hasKeyboardListener()) {
      break;
    }
    if (event.key.keysym.sym == SDLK_a) {
      ++speed_;
    }
    if (event.key.keysym.sym == SDLK_d) {
      --speed_;
      if (speed_ == 0) {
        speed_ = 1;
      }
    }
    break;
  case SDL_VIDEORESIZE:
    Renderer::getInstance().resize(event.resize.w, event.resize.h);
    break;
  default:
    break;
  }
}

void BoxCar::close()
{
  SDL_Quit();
}

void BoxCar::quit()
{
  state_ = Quit;
  stateRunnig_ = false;
}

void BoxCar::toggleMenu()
{
  state_ = Menu;
  stateRunnig_ = false;
}

void BoxCar::toggleGame()
{
  state_ = Game;
  stateRunnig_ = false;
}

void BoxCar::toggleCredits()
{
  state_ = ShowCredits;
  stateRunnig_ = false;
}

bool BoxCar::initCredits()
{
  stateRunnig_ = true;
  Credits* credits = new Credits(Rect(0,0,1,1));
  Renderer::getInstance().addWidget(credits);
  return true;
}

bool BoxCar::finishCredits()
{
  Renderer::getInstance().clearWindows();
  return true;
}

void BoxCar::togglePause()
{
  paused_ = !paused_;
}

void BoxCar::birthModeClick(void* param)
{
  int mode = *((int*)(&param));
  assert (mode >= 0 && mode < World::LastMode);
  birthMode_ = (World::BirthPosition)mode;
  for (int i=World::TrackStart; i< World::LastMode; ++i) {
    if (i == mode) {
      birthModeButtons_[i]->setHighlighted(true);
    } else {
      birthModeButtons_[i]->setHighlighted(false);
    }
  }
}

#ifdef WIN32
int __stdcall WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
#else
int main(int argc, char** argv)
#endif
{
  Logger::getInstance().log(INFO_LOG_NAME, "Application started. Hello, World!");
  Logger::getInstance().log(INFO_LOG_NAME, CString("Application version: ") + Version + " " + CString(BUILD_STR));
#if defined(WIN32) && defined(DEBUG)
  AllocConsole();
  FILE* stream = NULL;
  errno_t err = freopen_s(&stream, "CON", "w", stdout);
#endif


  BoxCar& game = BoxCar::getInstance();
  bool res = game.init();
  if (res) {
    Logger::getInstance().log(INFO_LOG_NAME, "Initialization successfully completed");
    game.run();
  } else {
    Logger::getInstance().log(ERROR_LOG_NAME, "Something went horribly wrong. Look above for details.");
  }

#if defined(WIN32) && defined(DEBUG)
  FreeConsole();
#endif
  Logger::getInstance().log(INFO_LOG_NAME, "Application ended. Bye!");
}
