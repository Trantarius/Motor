#include <iostream>
#include <stdexcept>
#include "main.hpp"
#include "util/glm.hpp"
#include "Render.hpp"
#include "Mesh.hpp"

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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
  glClear(GL_COLOR_BUFFER_BIT);

  renderAll();

  glfwSwapBuffers(window);
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

  Shader shader("src/shaders/pass.v.glsl","src/shaders/color.f.glsl");
  shader.setUniform("color",vec4(1,1,1,1));
  vec3 verts[]{
    vec3(-0.5,-0.5,0),
    vec3(0,0.5,0),
    vec3(0.5,-0.5,0)
  };
  MeshData triangle(Bloc<vec3>(verts,3));

  Mesh mesh;
  mesh.mesh_data=triangle;
  mesh.shader=shader;

  while (!shouldQuit){
    mainLoop();
  }

  terminate();
  return 0;
}
