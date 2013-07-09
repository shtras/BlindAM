#pragma once

struct TextureParams
{
  int textureLeft;
  int textureTop;
  int textureWidth;
  int textureHeight;
  int borderLeft;
  int borderTop;
  int borderRight;
  int borderDown;
};

class TexturedWidget
{
public:
  ENGINE_API TexturedWidget(Rect size);
  ENGINE_API ~TexturedWidget();
  ENGINE_API void render();
  ENGINE_API void initDimensions(int width, int height, int texWidth, int texHeight);
  ENGINE_API void invalidateDimensions();
  ENGINE_API void init(TextureParams params);
  ENGINE_API void resize(Rect size);
private:
  TexturedWidget();
  Rect size_;
  TextureParams texParams_;
  Rect topLeft_;
  Rect topLeftTex_;
  Rect left_;
  Rect leftTex_;
  Rect bottomLeft_;
  Rect bottomLeftTex_;
  Rect top_;
  Rect topTex_;
  Rect bottom_;
  Rect bottomTex_;
  Rect topRight_;
  Rect topRightTex_;
  Rect right_;
  Rect rightTex_;
  Rect bottomRight_;
  Rect bottomRightTex_;
  Rect body_;
  Rect bodyTex_;
  bool dimensionsInitialized_;
};


class Widget
{
public:
  ENGINE_API enum Align {NoAlign, LeftAlign, UpAlign, RightAlign, DownAlign};
  ENGINE_API Widget(Rect size);
  ENGINE_API virtual ~Widget();
  ENGINE_API void renderFrameRec();
  ENGINE_API virtual void customFrameRender() {}
  ENGINE_API void renderContRec();
  ENGINE_API virtual void render() {}
  ENGINE_API void initRegular(TextureParams params);
  ENGINE_API void initHovered(TextureParams params);
  ENGINE_API void initPressed(TextureParams params);
  //Base events
  ENGINE_API void lmDown();
  ENGINE_API void rmDown();
  ENGINE_API void lmUp();
  ENGINE_API void rmUp();
  ENGINE_API void click();
  ENGINE_API void rightClick();
  ENGINE_API void hoverEnter();
  ENGINE_API void hoverExit();
  ENGINE_API void drag();
  ENGINE_API void stopDrag();
  ENGINE_API void drop(Widget* w);
  ENGINE_API void mouseMove();
  ENGINE_API void mouseWheelScroll(int direction);
  ENGINE_API void keyDown(SDL_keysym details);
  ENGINE_API void keyboardFocusGain();
  ENGINE_API void keyboardFocusLose();
  //User events
  ENGINE_API virtual void onLMDown() {}
  ENGINE_API virtual void onRMDown() {}
  ENGINE_API virtual void onLMUp() {}
  ENGINE_API virtual void onRMUp() {}
  ENGINE_API virtual void onClick() {}
  ENGINE_API virtual void onRightClick() {}
  ENGINE_API virtual void onHoverEnter() {}
  ENGINE_API virtual void onHoverExit() {}
  ENGINE_API virtual void onDrag() {}
  ENGINE_API virtual void onStopDrag() {}
  ENGINE_API virtual void onDrop(Widget* w) {}
  ENGINE_API virtual void onMouseMove() {}
  ENGINE_API virtual void onMouseWheelScroll(int direction) {}
  ENGINE_API virtual void onResize() {}
  ENGINE_API virtual void onKeyDown(SDL_keysym details) {}
  ENGINE_API virtual void onKeyboardFocusGain() {}
  ENGINE_API virtual void onKeyboardFocusLose() {}
  //Child care
  ENGINE_API void addWidget(Widget* widget);
  ENGINE_API void removeWidget(Widget* widget);
  ENGINE_API void clear();
  ENGINE_API bool handleEvent(SDL_Event& event, float fx, float fy);
  ENGINE_API void resize(Rect size);
  ENGINE_API Rect getSize() {return size_;}
  ENGINE_API void setHighlighted(bool value);
  ENGINE_API bool isHighlighted() {return highlighted_;}
  ENGINE_API bool isVisible() {return visible_;}
  ENGINE_API void setVisible(bool value) {visible_ = value;}
  ENGINE_API void setToolTip(CString toolTip) {toolTip_ = toolTip;}
  ENGINE_API CString getToolTip() {return toolTip_;}
  ENGINE_API void setAlign(Align align) {align_ = align;}
protected:
  Rect size_;
  Rect finalSize_;
  bool deploying_;
  bool clickable_;
  bool draggable_;
  bool scrollTransparrent_; //Widget does not handle scroll events
  list<Widget*> children_;
  TexturedWidget* regularTexture_;
  TexturedWidget* hoveredTexture_;
  TexturedWidget* pressedTexture_;
  TexturedWidget* activeTexture_;
  Align align_;
  bool visible_;
  bool listensKeyboard_;
private:
  Widget();
  bool hovered_;
  bool pressed_;
  bool rightPressed_;
  bool dragged_;
  bool highlighted_;
  CString toolTip_;
  int id_;
};
