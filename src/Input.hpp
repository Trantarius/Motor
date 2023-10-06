#pragma once
#include "util/strings.hpp"
#include "keys.hpp"
#include "util/glm.hpp"
#include "main.hpp"

class Window;

class Input{
public:
  enum CursorMode{NORMAL=0x00034001,HIDDEN=0x00034002,DISABLED=0x00034003};
private:
  CursorMode cursorMode=NORMAL;
  dvec2 mouseVel{};
  dvec2 mousePos{};
  double lastMouseTime=time();
  Map<Key,bool> keyStates;
  Window& window;

  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
  static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
public:
  static string keyName(Key key);
  bool getKey(Key key);

  void setCursorMode(CursorMode to);
  CursorMode getCursorMode();

  vec2 getMousePos(){return mousePos;}
  vec2 getMouseVel(){return mouseVel;}

  Input(Window& win);
};
