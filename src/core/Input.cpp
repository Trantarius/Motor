#include "Input.hpp"
#include "main.hpp"
#include "Window.hpp"
#include "util/time.hpp"

string Input::keyName(Key key){
  return keyNames.at(key);
}

bool Input::getKey(Key key){
  return keyStates[key];
}

void Input::setCursorMode(CursorMode to){
  cursorMode=to;
  glfwSetInputMode(Window::glfw(),GLFW_CURSOR,to);
}
Input::CursorMode Input::getCursorMode(){
  return cursorMode;
}

void Input::update(){
  double now=time();
  fvec2 mrel=mouse_pos-last_mouse_pos;
  double dt=now-last_update;
  fvec2 mvel=mrel/dt;
  if(len(mvel)>10000){
    mvel=fvec2(0,0);
  }
  mouse_vel=mvel;
  last_update=now;
  last_mouse_pos=mouse_pos;
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
  if(action==GLFW_PRESS){
    bool is_press = action==GLFW_PRESS;
    keyStates[(Key)key]=is_press;
    List<CallablePtr<void(void)>> tasks;
    for(const CallablePtr<void(Key,bool)>& listener : generic_listeners){
      tasks.push_back(BoundCallable<void(Key,bool)>(listener, (Key)key, is_press));
    }
    for(const CallablePtr<void(bool)>& listener : key_listeners[(Key)key]){
      tasks.push_back(BoundCallable<void(bool)>(listener, is_press));
    }
    if(is_press){
      for(const CallablePtr<void(void)>& listener : keypress_listeners[(Key)key]){
        tasks.push_back(listener);
      }
    }
    input_event_pool.add_tasks(tasks);
  }
}

void Input::cursorPosCallback(GLFWwindow* window, double xpos, double ypos){
  mouse_pos = fvec2(xpos,ypos);
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
  keyStates[(Key)button]=(action==GLFW_PRESS);
}


void Input::init(){
  glfwSetKeyCallback(Window::glfw(),Input::keyCallback);
  glfwSetCursorPosCallback(Window::glfw(),Input::cursorPosCallback);

  for(auto pr : keyNames){
    keyStates.emplace(pr.first,false);
    key_listeners.emplace(pr.first,Cycle<void(bool)>());
    keypress_listeners.emplace(pr.first,Cycle<void(void)>());
  }
}

void Input::addGenericListener(const CallablePtr<void(Key,bool)>& listener){
  generic_listeners.insert(listener);
}

void Input::removeGenericListener(const CallablePtr<void(Key,bool)>& listener){
  generic_listeners.erase(listener);
}

void Input::addKeyListener(Key key,const CallablePtr<void(bool)>& listener){
  key_listeners[key].insert(listener);
}
void Input::removeKeyListener(Key key,const CallablePtr<void(bool)>& listener){
  key_listeners[key].erase(listener);
}

void Input::addKeypressListener(Key key,const CallablePtr<void(void)>& listener){
  keypress_listeners[key].insert(listener);
}
void Input::removeKeypressListener(Key key,const CallablePtr<void(void)>& listener){
  keypress_listeners[key].erase(listener);
}
