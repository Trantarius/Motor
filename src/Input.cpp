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
  Window::fromGLFW(window)->input.mousePos=vec2(xpos,ypos);
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
