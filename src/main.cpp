#include <iostream>
#include <stdexcept>
#include "main.hpp"
#include "util/glm.hpp"
#include "Render.hpp"
#include "Mesh.hpp"
#include "util/gl_enum_names.hpp"
#include <glm/ext.hpp>
#include "Input.hpp"
#include "Window.hpp"

Window* Main::window=nullptr;
Render* Main::render=nullptr;
Input* Main::input=nullptr;
bool shouldQuit=false;

struct InitFailedError : std::runtime_error{
  InitFailedError(std::string what):std::runtime_error(what){}
};

void openGLDebugCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message, const void* userparam){
  if(severity==GL_DEBUG_SEVERITY_NOTIFICATION){
    return;
  }
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

  Main::window=new Window();
  Main::input=&Main::window->input;
  Main::render=&Main::window->render;

  glfwMakeContextCurrent(Main::window->glfw());

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    glfwTerminate();
    throw InitFailedError("Failed to initialize GLAD");
  }

  glDebugMessageCallback(openGLDebugCallback,NULL);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

void mainLoop(){
  while(!shouldQuit){
    glfwPollEvents();
    Main::render->render();
    if(glfwWindowShouldClose(Main::window->glfw())){
      quit();
    }
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

  Main::render->camera=Unique<Camera>( new PerspectiveCamera());
  Main::render->camera->renderer=Main::render;

  Shader shader("src/shaders/mesh.vert","src/shaders/normals.frag");
  MeshData suzanne = MeshData::readOBJ("suzanne.obj");
  MeshData triangle = MeshData(Bloc<vec3>(vec3(-0.5,-0.5,-1),vec3(0,0.5,-1),vec3(0.5,-0.5,-1)));

  Mesh mesh;
  mesh.mesh_data=suzanne;
  mesh.shader=shader;
  mesh.position.z-=5;
  Main::render->add(&mesh);


  mainLoop();

  terminate();
  return 0;
}
