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

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
  if(action==GLFW_PRESS){
    Window::fromGLFW(window)->input.keyStates[(Key)key]=true;
  }else if(action==GLFW_RELEASE){
    Window::fromGLFW(window)->input.keyStates[(Key)key]=false;
  }
}

void Input::cursorPosCallback(GLFWwindow* window, double xpos, double ypos){
  double now=time();
  double deltaT=now-Window::fromGLFW(window)->input.lastMouseTime;
  Window::fromGLFW(window)->input.lastMouseTime=now;

  dvec2 deltaP = dvec2(xpos,ypos)-Window::fromGLFW(window)->input.mousePos;
  Window::fromGLFW(window)->input.mousePos=dvec2(xpos,ypos);
  Window::fromGLFW(window)->input.mouseVel=deltaP/deltaT;
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
  }
}
