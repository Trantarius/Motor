#include <iostream>
#include <stdexcept>
#include "main.h"

GLFWwindow* window=nullptr;
bool shouldQuit=false;

struct InitFailedError : std::runtime_error{
  InitFailedError(std::string what):std::runtime_error(what){}
};

void init(){
  if(!glfwInit()){
    throw InitFailedError("glfwInit failed");
  }

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

int main(){
  init();

  while (!shouldQuit){
    mainLoop();
  }

  terminate();
  return 0;
}
