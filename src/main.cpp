#include <iostream>
#include <stdexcept>
#include "main.hpp"
#include "util/glm.hpp"
#include "Render.hpp"
#include "Mesh.hpp"
#include "util/gl_enum_names.hpp"
#include <glm/ext.hpp>

GLFWwindow* window=nullptr;
bool shouldQuit=false;

struct InitFailedError : std::runtime_error{
  InitFailedError(std::string what):std::runtime_error(what){}
};

void windowResizeCallback(GLFWwindow* window, int width, int height){
  glViewport(0,0,width,height);
  Render::main.camera.projection=perspective(radians(45.0f),(float)width/height,0.1f,100.0f);
}

void openGLDebugCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message, const void* userparam){
  printerr("GLDebug:");
  printerr("\tsource: "+glEnumName(source));
  printerr("\ttype: "+glEnumName(type));
  printerr("\tid: ",id);
  printerr("\tseverity: "+glEnumName(severity));
  printerr("\tmessage: ",message);
}

void init(){
  if(!glfwInit()){
    throw InitFailedError("glfwInit failed");
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

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
  glDebugMessageCallback(openGLDebugCallback,NULL);
  glEnable(GL_DEPTH_TEST);
}

void mainLoop(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Render::main.render();

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
Render myrend;
int main(){
  init();

  Shader shader("src/shaders/mesh.vert","src/shaders/normals.frag");
  MeshData suzanne = MeshData::readOBJ("suzanne.obj");
  MeshData triangle = MeshData(Bloc<vec3>(vec3(-0.5,-0.5,-1),vec3(0,0.5,-1),vec3(0.5,-0.5,-1)));

  Mesh mesh;
  mesh.mesh_data=suzanne;
  mesh.shader=shader;
  mesh.translate(dvec3(0,0,-5));


  while (!shouldQuit){
    mainLoop();
  }

  terminate();
  return 0;
}
