#include "Input.hpp"
#include "main.hpp"
#include "Window.hpp"

string Input::keyName(Key key){
  return keyNames.at(key);
}

bool Input::getKey(Key key){
  return keyStates[key];
}

void Input::setCursorMode(CursorMode to){
  cursorMode=to;
  glfwSetInputMode(window.glfw(),GLFW_CURSOR,to);
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
    Window::fromGLFW(window)->input.keyStates[(Key)key]=true;
    Window::fromGLFW(window)->input.generic_listeners.cycle((Key)key,true);
    Window::fromGLFW(window)->input.key_listeners[(Key)key].cycle(true);
    Window::fromGLFW(window)->input.keypress_listeners[(Key)key].cycle();
  }else if(action==GLFW_RELEASE){
    Window::fromGLFW(window)->input.keyStates[(Key)key]=false;
    Window::fromGLFW(window)->input.generic_listeners.cycle((Key)key,false);
    Window::fromGLFW(window)->input.key_listeners[(Key)key].cycle(false);
  }
}

void Input::cursorPosCallback(GLFWwindow* window, double xpos, double ypos){
  Window::fromGLFW(window)->input.mouse_pos=fvec2(xpos,ypos);
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
  if(action==GLFW_PRESS){
    Window::fromGLFW(window)->input.keyStates[(Key)button]=true;
  }else if(action==GLFW_RELEASE){
    Window::fromGLFW(window)->input.keyStates[(Key)button]=false;
  }
}


Input::Input(Window& win):window(win){
  glfwSetKeyCallback(window.glfw(),Input::keyCallback);
  glfwSetCursorPosCallback(window.glfw(),Input::cursorPosCallback);

  for(auto pr : keyNames){
    keyStates.emplace(pr.first,false);
    key_listeners.emplace(pr.first,Cycle<void(bool)>());
    keypress_listeners.emplace(pr.first,Cycle<void(void)>());
  }
}

void Input::addGenericListener(SafeFunc<void(Key,bool)> listener){
  generic_listeners.add(listener);
}

void Input::removeGenericListener(SafeFunc<void(Key,bool)> listener){
  generic_listeners.remove(listener);
}

void Input::addKeyListener(Key key,SafeFunc<void(bool)> listener){
  key_listeners[key].add(listener);
}
void Input::removeKeyListener(Key key,SafeFunc<void(bool)> listener){
  key_listeners[key].remove(listener);
}

void Input::addKeypressListener(Key key,SafeFunc<void(void)> listener){
  keypress_listeners[key].add(listener);
}
void Input::removeKeypressListener(Key key,SafeFunc<void(void)> listener){
  keypress_listeners[key].remove(listener);
}
