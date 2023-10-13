#include <iostream>
#include <stdexcept>
#include "main.hpp"
#include "util/math.hpp"
#include "Render.hpp"
#include "Mesh.hpp"
#include "util/gl_enum_names.hpp"
#include "Input.hpp"
#include "Window.hpp"
#include <chrono>
#include "Updater.hpp"
#include "OrbitCamera.hpp"
#include "util/rand.hpp"
#include "util/id.hpp"
#include "Texture.hpp"

Window* Main::window=nullptr;
Render* Main::render=nullptr;
Input* Main::input=nullptr;
Updater* Main::updater=nullptr;
bool should_quit=false;

void init();
void mainLoop();
void terminate();

uint64_t nt = nanotime();
int _main(){

  OrbitCamera* camera=new OrbitCamera();
  Main::render->camera=Unique<Camera>(camera);
  Main::render->camera->renderer=Main::render;
  Main::updater->add(*camera);

  Main::input->setCursorMode(Input::DISABLED);

  Shader shader("src/shaders/mesh.v.glsl","src/shaders/tex.f.glsl");
  MeshData suzanne = MeshData::readOBJ("assets/cube.obj");
  MeshData triangle = MeshData(Bloc<fvec3>(fvec3(-0.5,-0.5,-1),fvec3(0,0.5,-1),fvec3(0.5,-0.5,-1)));

  Mesh mesh;
  mesh.mesh_data=suzanne;
  mesh.shader=shader;
  Main::render->add(mesh);

  Texture tex=Texture::readPNG("assets/awesomeface.png");
  shader.setUniform("tex"_id,tex);

  Array<Shader::UniformInfo> unis=shader.getUniforms();
  for(Shader::UniformInfo& uni : unis){
    print(uni);
  }

  mainLoop();
  return 0;
}

int main(){
  init();
  int r=_main();
  terminate();
}




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
  Main::updater=new Updater();

  glfwMakeContextCurrent(Main::window->glfw());
  glfwSwapInterval(1);//enable Vsync
  if(glfwRawMouseMotionSupported()){
    glfwSetInputMode(Main::window->glfw(),GLFW_RAW_MOUSE_MOTION,GLFW_TRUE);
  }

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

  size_t frame_counter=0;
  double last_frame=time();

  while(!should_quit){
    glfwPollEvents();
    Main::input->update();
    Main::updater->cycle(Main::updater);
    Main::render->cycle(Main::render);
    if(glfwWindowShouldClose(Main::window->glfw())){
      quit();
    }

    frame_counter++;
    double now=time();
    if(now-last_frame>1.0){
      last_frame=now;
      frame_counter=0;
    }
  }
}





void quit(){
  should_quit=true;
}

void terminate(){
  glfwTerminate();
}






const std::chrono::steady_clock::time_point engine_start_time = std::chrono::steady_clock::now();
double time(){
  auto now=std::chrono::steady_clock::now();
  std::chrono::duration<double> ret = now - engine_start_time;
  return ret.count();
}

uint64_t nanotime(){
  assert(std::chrono::steady_clock::period::den>=1000000000);
  auto now=std::chrono::steady_clock::now();
  std::chrono::steady_clock::duration diff = now - engine_start_time;
  std::chrono::nanoseconds ret = std::chrono::duration_cast<std::chrono::nanoseconds>(diff);
  return ret.count();
}
