#pragma once
#include "util/strings.hpp"
#include "keys.hpp"
#include "util/math.hpp"
#include "util/funcy.hpp"
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

  List<SafeFunc<void(Key,bool)>> generic_listeners;
  Map<Key,List<SafeFunc<void(bool)>>> key_listeners;
  Map<Key,List<SafeFunc<void(void)>>> keypress_listeners;


  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
  static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
public:
  static string keyName(Key key);
  bool getKey(Key key);

  void setCursorMode(CursorMode to);
  CursorMode getCursorMode();

  fvec2 getMousePos(){return mousePos;}
  fvec2 getMouseVel(){return mouseVel;}

  void addGenericListener(SafeFunc<void(Key,bool)> listener);
  void removeGenericListener(SafeFunc<void(Key,bool)> listener);

  void addKeyListener(Key key,SafeFunc<void(bool)> key_listener);
  void removeKeyListener(Key key,SafeFunc<void(bool)> key_listener);

  void addKeypressListener(Key key,SafeFunc<void(void)> keypress_listener);
  void removeKeypressListener(Key key,SafeFunc<void(void)> keypress_listener);

  Input(Window& win);
};
