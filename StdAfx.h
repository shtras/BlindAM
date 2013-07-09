#pragma once

#ifdef WIN32
#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport) 
#else
#define ENGINE_API __declspec(dllimport) 
#endif
#else
#define ENGINE_API
#endif

#ifdef WIN32
#include <Windows.h>
#else
#include <limits.h>
#endif //WIN32
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <algorithm>
#include <assert.h>
using namespace std;

#ifdef DEBUG
#define checkError(n) checkErrorDebug(n)
#else
#define checkError(n)
#endif
#define INFO_LOG_NAME "info.log"
#define ERROR_LOG_NAME "error.log"
#define PI (3.1415926536f)

#ifdef DEBUG
#define BUILD_STR "Debug"
#else
#define BUILD_STR "Release"
#endif

#include "CString.h"
#include "Logger.h"
#include "auxFuncs.h"
#include "Vector.h"
#include "Matrix.h"
#ifdef WIN32
#include "SDL.h"
#include "SDL_image.h"
#else
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif
#include <GL/gl.h>

#include "sigslot.h"
using namespace sigslot;

struct Rect
{
  Rect():left(0),top(0),width(0),height(0){}
  Rect (float left, float top, float width, float height):
    left(left),top(top),width(width),height(height){}
  Rect (double left, double top, double width, double height):
    left((float)left),top((float)top),width((float)width),height((float)height){}
  Rect (int left, int top, int width, int height):
    left((float)left),top((float)top),width((float)width),height((float)height){}
  float left;
  float top;
  float width;
  float height;
};

struct FCoord
{
  FCoord():x(0.0f),y(0.0f){}
  FCoord(float x, float y):x(x),y(y){}
  FCoord(double x, double y):x((float)x),y((float)y){}
  FCoord(int x, int y):x((float)x),y((float)y){}
  float x;
  float y;
};

struct DCoord
{
  DCoord():x(0),y(0){}
  DCoord(int x, int y): x(x),y(y){}
  bool operator == (const DCoord& other) const {return x == other.x && y == other.y;}
  int x;
  int y;
};

struct Position
{
  bool operator==(const Position& other) {
    return coord.x == other.coord.x && coord.y == other.coord.y && deckIdx == other.deckIdx;
  }
  bool operator!=(const Position& other) {
    return !(*this == other);
  }
  DCoord coord;
  int deckIdx;
};

enum Direction {Left, Up, Right, Down, UpStairs, DownStairs};
