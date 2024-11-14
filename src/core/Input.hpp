#pragma once
#include "defs/keys.hpp"
#include "util/vec.hpp"
//#include "util/Cycle.hpp"
#include "TaskPool.hpp"
#include <set>
#include <map>

struct GLFWwindow;
class Window;

class Input{
public:
  enum CursorMode{NORMAL=0x00034001,HIDDEN=0x00034002,DISABLED=0x00034003};
private:
  inline static std::mutex mtx;
  inline static CursorMode cursorMode=NORMAL;
  inline static fvec2 mouse_pos{};
  inline static fvec2 mouse_vel{};
  inline static double last_update=0;
  inline static fvec2 last_mouse_pos{};
  inline static Map<Key,bool> keyStates;
  //inline static Window& window;

  inline static SingleThreadPool input_event_pool;

  inline static std::set<CallablePtr<void(Key,bool)>> generic_listeners;
  inline static std::map<Key,std::set<CallablePtr<void(bool)>>> key_listeners;
  inline static std::map<Key,std::set<CallablePtr<void(void)>>> keypress_listeners;


  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
  static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
public:
  static string keyName(Key key);
  static bool getKey(Key key);

  static void setCursorMode(CursorMode to);
  static CursorMode getCursorMode();

  static fvec2 getMousePos(){return mouse_pos;}
  static fvec2 getMouseVel(){return mouse_vel;}
  static void update();

  static void addGenericListener(const CallablePtr<void(Key,bool)>& listener);
  static void removeGenericListener(const CallablePtr<void(Key,bool)>& listener);

  static void addKeyListener(Key key,const CallablePtr<void(bool)>& key_listener);
  static void removeKeyListener(Key key,const CallablePtr<void(bool)>& key_listener);

  static void addKeypressListener(Key key,const CallablePtr<void(void)>& keypress_listener);
  static void removeKeypressListener(Key key,const CallablePtr<void(void)>& keypress_listener);

  static void init();

  //Input(Window& win);
};
