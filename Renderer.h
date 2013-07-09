#pragma once
#include "TexturedWidget.h"
#include "GlobalWindow.h"
#include "Window.h"

struct TextToRender
{
  float fx;
  float fy;
  float size_;
  CString text;
  Vector4 color;
  bool dimOnHover;
};

struct TexturedVertex
{
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat u;
  GLfloat v;
  GLubyte r;
  GLubyte g;
  GLubyte b;
  GLubyte a;
};

struct SimpleVertex
{
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLubyte r;
  GLubyte g;
  GLubyte b;
  GLubyte a;
};

#define MAX_VERTS (4096*3)

class Renderer
{
public:
  ENGINE_API static Renderer& getInstance();
  ENGINE_API bool init();
  ENGINE_API void clear();
  ENGINE_API void render();
  ENGINE_API void renderEnd();
  ENGINE_API void renderText(float x, float y, CString text);
  ENGINE_API void drawTexRect(Rect pos, GLuint texID, Rect texPos, int rotation = 0);
  ENGINE_API void drawLine(FCoord from, FCoord to);
  ENGINE_API void drawSquare(Rect size);
  ENGINE_API void drawCircle(FCoord center, float radius, int numChunks = 15);
  ENGINE_API void setTextSize(float size) {textSize_ = size;}
  ENGINE_API void setColor(Vector4 color);
  ENGINE_API void setColor(Vector3 color);
  ENGINE_API void resetColor();
  ENGINE_API GLuint getGUITex() {return gui_;}
  ENGINE_API GLuint getTilesTex() {return tiles_;}
  ENGINE_API GLuint getBGTex() {return bgTex_;}
  ENGINE_API GLuint getHeadsTex() {return headsTex_;}
  ENGINE_API GLuint getSolarTilesTex() {return solarTilesTex_;}
  ENGINE_API DCoord getSolarTilesTexSize() {return solarTilesTexSize_;}
  ENGINE_API DCoord getHeadsTexSize() {return headsTexSize_;}
  ENGINE_API void initWidgetDimensions(TexturedWidget* widget);
  ENGINE_API void handleGUIEvent(SDL_Event& event);
  ENGINE_API void handleKeyboardEvent(SDL_Event& event);
  ENGINE_API void setDraggedWidget(Widget* w);
  ENGINE_API Widget* getDraggedWidget() {return draggedWidget_;}
  ENGINE_API void setFloatingWidget(Widget* w);
  ENGINE_API Widget* getFloatingWidget() {return floatingWidget_;}
  ENGINE_API void setKeyboardListener(Widget* w);
  ENGINE_API bool hasKeyboardListener() {return keyboardListner_ != NULL;}
  ENGINE_API bool renderingDragged() {return renderingDragged_;}
  ENGINE_API float getOffsetX() {return startMouseX_ - mouseX_;}
  ENGINE_API float getOffsetY() {return startMouseY_ - mouseY_;}
  ENGINE_API float getMouseX() {return mouseX_;}
  ENGINE_API float getMouseY() {return mouseY_;}
  ENGINE_API int getWidth() {return width_;}
  ENGINE_API int getHeight() {return height_;}
  ENGINE_API float getCharWidth();
  ENGINE_API float getCharHeight();
  ENGINE_API void addWidget(Widget* window);
  ENGINE_API void clearWindows();
  ENGINE_API int getTilesTexWidth() {return tilesTexWidth_;}
  ENGINE_API int getTilesTexHeight() {return tilesTexHeight_;}
  ENGINE_API void setWidgetForTooltip(Widget* w) {renderTooltipFor_ = w;}
  ENGINE_API void resize(int width, int height);
  ENGINE_API void setFloatingWidgetWidthLimit(float value) {floatingWidgetWidthLimit_ = value;}
  ENGINE_API void setDrawBoundries(Rect boundries) {drawBoundries_ = boundries;}
  ENGINE_API void resetDrawBoundries();
  ENGINE_API void clampRect(Rect& pos, FCoord& tex1, FCoord& tex2, FCoord& tex3, FCoord& tex4);
  ENGINE_API float getAspect() {return aspect_;}
private:
  Renderer();
  ~Renderer();
  void renderToolTip(CString text);
  void loadFonts();
  void loadTexture(CString fileName, GLuint& tex);
  void loadTexture(CString fileName, GLuint& tex, int& width, int& height);
  void renderTextLines();
  void renderTextLine(TextToRender& ttr);
  void addTexVertex(float x, float y, float u, float v);
  void addSimpleVertex(float x, float y);
  void flushTexVerts();
  void flushSimpVerts();
  GLuint font_;
  GLuint gui_;
  GLuint tiles_;
  GLuint bgTex_;
  GLuint headsTex_;
  GLuint solarTilesTex_;
  DCoord solarTilesTexSize_;
  DCoord headsTexSize_;
  Vector4 color_;
  Vector4 lastColor_;
  TexturedVertex texVerts_[MAX_VERTS];
  SimpleVertex simpVerts_[MAX_VERTS];
  int currTexVertIdx_;
  int currSimpVertIdx_;
  GLuint activeTex_;
  GLuint logoTex_;
  int width_;
  int height_;
  float textSize_;
  int guiTexWidth_;
  int guiTexHeight_;
  int tilesTexHeight_;
  int tilesTexWidth_;

  list<TextToRender> linesToRender_;
  GlobalWindow* globalGUIWindow_;
  //Render at this offset
  float offsetX_;
  float offsetY_;
  //For drag-drop render
  Widget* draggedWidget_;
  float xAtStartDrag_;
  float yAtStartDrag_;
  float mouseX_;
  float mouseY_;
  float startMouseX_;
  float startMouseY_;
  bool renderingDragged_;
  int flushes_;
  int vertices_;
  Widget* renderTooltipFor_;
  TexturedWidget* toolTip_;
  Widget* floatingWidget_;
  Widget* keyboardListner_;
  map<char, float> fontMapX_;
  float floatingWidgetWidthLimit_;
  int charWidth_;
  int charHeight_;
  bool dimTextOnHover_;
  Rect drawBoundries_;
  float aspect_;
};