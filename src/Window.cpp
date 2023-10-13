#include "Window.hpp"
#include "GLFW/glfw3.h"

Map<GLFWwindow*,Window*> Window::ptr_map;

ivec2 Window::getSize() const{
  int x,y;
  glfwGetWindowSize(window,&x,&y);
  return ivec2(x,y);
}

void Window::setSize(ivec2 to){
  glfwSetWindowSize(window,to.x,to.y);
}

void framebufferResizeCallback(GLFWwindow* window, int width, int height){
  glfwMakeContextCurrent(window);
  glViewport(0,0,width,height);
}

GLFWwindow* make_new_window(){
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  GLFWwindow* ret=glfwCreateWindow(1024,600,"Hello",NULL,NULL);
  assert(ret);
  return ret;
}

Window::Window():window(make_new_window()){
  assert(window);
  glfwSetFramebufferSizeCallback(window,framebufferResizeCallback);
  ptr_map[window]=this;
}

Window* Window::fromGLFW(GLFWwindow* ptr){
  assert(ptr_map.contains(ptr));
  return ptr_map[ptr];
}
