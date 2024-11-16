#pragma once
#include "defs/keys.hpp"
#include "util/vec.hpp"
//#include "util/Cycle.hpp"
#include "TaskPool.hpp"
#include "defs/gl_defs.hpp"  // IWYU pragma: keep
#include <set>
#include <map>

class Input{
  inline static int cursor_mode=GLFW_CURSOR_NORMAL;
  inline static fvec2 mouse_pos{};
  inline static fvec2 mouse_vel{};
  inline static double last_update=0;
  inline static fvec2 last_mouse_pos{};
  inline static std::map<Key,bool> keyStates;
  //inline static Window& window;

  inline static TaskPool input_event_pool;

  inline static std::set<Callback<Key,bool>> generic_listeners;
  inline static std::map<Key,std::set<Callback<bool>>> key_listeners;
  inline static std::map<Key,std::set<Callback<>>> keypress_listeners;


  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
  static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
public:
  static std::string keyName(Key key);
  static bool getKey(Key key);

  static void setCursorMode(int to);
  static int getCursorMode();

  static fvec2 getMousePos(){return mouse_pos;}
  static fvec2 getMouseVel(){return mouse_vel;}
  static void update();

  static void addGenericListener(const Callback<Key,bool>& listener);
  static void removeGenericListener(const Callback<Key,bool>& listener);

  static void addKeyListener(Key key,const Callback<bool>& key_listener);
  static void removeKeyListener(Key key,const Callback<bool>& key_listener);

  static void addKeypressListener(Key key,const Callback<>& keypress_listener);
  static void removeKeypressListener(Key key,const Callback<>& keypress_listener);

  static void init();

  //Input(Window& win);
};
