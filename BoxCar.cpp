#include "StdAfx.h"
#include "BoxCar.h"
#include "Renderer.h"
#include "Button.h"
#include "Credits.h"

#include "Box2D/Box2D.h"

const char* Version = "0.2";

BoxCar::BoxCar():state_(Menu),stateRunnig_(false),paused_(true), world_(NULL), speed_(1), seedStr_("habrahabr.ru"), popSize_(100), breedThreshold_(10)
{
#ifdef DEBUG
  popSize_ = 20;
#endif
}

BoxCar::~BoxCar()
{

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

bool BoxCar::initMenu()
{
  Renderer& renderer = Renderer::getInstance();
  stateRunnig_ = true;
  Window* menuWindow = new Window(Rect(0.3, 0.1, 0.4, 0.8));
  renderer.addWidget(menuWindow);
  double top = 0.3;
  double height = 0.15;

  Text* seedText = new Text(Rect(0.1, 0.1, 0.2, 0.05));
  seedText->setCaption("Seed");
  menuWindow->addWidget(seedText);
  seedInput_ = new Input(Rect(0.1, 0.15, 0.2, 0.05));
  seedInput_->setCaption(seedStr_);
  menuWindow->addWidget(seedInput_);

  Text* popSizeText = new Text(Rect(0.35, 0.1, 0.2, 0.05));
  popSizeText->setCaption("Population size");
  menuWindow->addWidget(popSizeText);
  popSizeInput_ = new Input(Rect(0.35, 0.15, 0.2, 0.05));
  popSizeInput_->setCaption(CString(popSize_));
  menuWindow->addWidget(popSizeInput_);

  Text* breedThresholdText = new Text(Rect(0.6, 0.1, 0.2, 0.05));
  breedThresholdText->setCaption("Breed threshold");
  menuWindow->addWidget(breedThresholdText);
  breedThresholdInput_ = new Input(Rect(0.6, 0.15, 0.2, 0.05));
  breedThresholdInput_->setCaption(CString(breedThreshold_));
  menuWindow->addWidget(breedThresholdInput_);

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
  Renderer::getInstance().clearWindows();
  return true;
}

bool BoxCar::initGame()
{
  paused_ = true;
  stateRunnig_ = true;

  world_ = new World(Rect(0,0,1,1), seed_, popSize_, breedThreshold_);
  Renderer::getInstance().addWidget(world_);
  return true;
}

bool BoxCar::finishGame()
{
  Renderer::getInstance().clearWindows();
  world_ = NULL;
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

#ifdef WIN32
int __stdcall WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
#else
int main(int argc, char** argv)
#endif
{
  Logger::getInstance().log(INFO_LOG_NAME, "Application started. Hello, World!");
  Logger::getInstance().log(INFO_LOG_NAME, CString("Application version: ") + Version + CString(BUILD_NUM) + " " + CString(BUILD_STR));
#if defined(WIN32) && defined(DEBUG)
  AllocConsole();
  FILE* stream = NULL;
  errno_t err = freopen_s(&stream, "CON", "w", stdout);
#endif

  {
    b2Vec2 gravity (0.0f, -10.0f);
    b2World world(gravity);
    b2BodyDef def1;
    def1.position.Set(0.0f, -10.0f);
    b2Body* body = world.CreateBody(&def1);
    
    int a = 0;
  }

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
