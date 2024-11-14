#include "Window.hpp"
#include "defs/gl_defs.hpp"

ivec2 Window::getSize(){
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

void Window::init(){
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  window=glfwCreateWindow(1024,600,"Hello",NULL,NULL);
  assert(window);
  glfwSetFramebufferSizeCallback(window,framebufferResizeCallback);
}
