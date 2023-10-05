#include "Window.hpp"
#include "GLFW/glfw3.h"

Map<GLFWwindow*,Window*> Window::ptrMap;

uvec2 Window::getSize() const{
  int x,y;
  glfwGetWindowSize(window,&x,&y);
  return uvec2(x,y);
}

void Window::setSize(uvec2 to){
  glfwSetWindowSize(window,to.x,to.y);
}

void framebufferResizeCallback(GLFWwindow* window, int width, int height){
  glfwMakeContextCurrent(window);
  glViewport(0,0,width,height);
}

GLFWwindow* make_new_window(){
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  GLFWwindow* ret=glfwCreateWindow(1024,600,"Hello",NULL,NULL);
  assert(ret);
  return ret;
}

Window::Window():window(make_new_window()){
  assert(window);
  glfwSetFramebufferSizeCallback(window,framebufferResizeCallback);
  ptrMap[window]=this;
}

Window* Window::fromGLFW(GLFWwindow* ptr){
  assert(ptrMap.contains(ptr));
  return ptrMap[ptr];
}
