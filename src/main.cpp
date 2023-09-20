#include <iostream>
#include <stdexcept>
#include "main.hpp"
#include "util/glm.hpp"

GLFWwindow* window=nullptr;
bool shouldQuit=false;

struct InitFailedError : std::runtime_error{
  InitFailedError(std::string what):std::runtime_error(what){}
};

void windowResizeCallback(GLFWwindow* window, int width, int height){
  glViewport(0,0,width,height);
}

void init(){
  if(!glfwInit()){
    throw InitFailedError("glfwInit failed");
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window=glfwCreateWindow(640,480,"Hello",NULL,NULL);
  if(!window){
    glfwTerminate();
    throw InitFailedError("glfwCreateWindow failed");
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    glfwTerminate();
    throw InitFailedError("Failed to initialize GLAD");
  }

  glfwSetFramebufferSizeCallback(window,windowResizeCallback);
}

void mainLoop(){
  /* Render here */
  glClear(GL_COLOR_BUFFER_BIT);

  /* Swap front and back buffers */
  glfwSwapBuffers(window);

  /* Poll for and process events */
  glfwPollEvents();

  if(glfwWindowShouldClose(window)){
    quit();
  }
}

void quit(){
  shouldQuit=true;
}

void terminate(){
  glfwTerminate();
}

struct foo{
  foo(std::initializer_list<int> li){}
};

int main(){
  init();

  glm::vec3 av;
  glm::vec3 fv{1,2,3};
  glm::vec3 fv2{4,5,6};
  print(fv+fv2);
  print(av);

  foo myfoo{1,2,3,4};
  glm::mat2 mymat=glm::mat2();
  print(mymat);
  while (!shouldQuit){
    mainLoop();
  }

  terminate();
  return 0;
}
