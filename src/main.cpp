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
#include "util/strings.hpp"

Window* Main::window=nullptr;
Render* Main::render=nullptr;
Input* Main::input=nullptr;
Updater* Main::updater=nullptr;
bool should_quit=false;

void init();
void mainLoop();
void terminate();

bool debuggerIsAttached();

uint64_t nt = nanotime();
int _main(){

  if(!debuggerIsAttached()){
    Main::input->setCursorMode(Input::DISABLED);
  }

  OrbitCamera* camera=new OrbitCamera();
  Main::render->camera=Unique<Camera>(camera);
  Main::render->camera->renderer=Main::render;
  Main::updater->add(*camera);

  Shader shader("/mesh.v.glsl","/lighttest.f.glsl");
  MeshData suzanne = MeshData::readOBJ("assets/suzanne.obj");
  MeshData triangle = MeshData(Bloc<fvec3>(fvec3(-0.5,-0.5,-1),fvec3(0,0.5,-1),fvec3(0.5,-0.5,-1)));

  Mesh mesh;
  mesh.mesh_data=suzanne;
  mesh.shader=shader;
  Main::render->add(mesh);

  Light light;
  light.type=Light::POINT;
  light.position=dvec3(1,1,1);
  Main::render->addLight(light);

  mainLoop();
  return 0;
}

int main(){
  if(debuggerIsAttached()){
    setgetPrintColor(0);
    print("Running inside GDB");
  }else{
    setgetPrintColor(1);
  }
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

  GLFWwindow* winptr=Window::make_new_window();

  glfwMakeContextCurrent(winptr);
  glfwSwapInterval(1);//enable Vsync
  if(glfwRawMouseMotionSupported()){
    glfwSetInputMode(winptr,GLFW_RAW_MOUSE_MOTION,GLFW_TRUE);
  }

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    glfwTerminate();
    throw InitFailedError("Failed to initialize GLAD");
  }

  glDebugMessageCallback(openGLDebugCallback,NULL);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  Shader::loadAllShaderFiles("src/shaders");

  Main::window=new Window(winptr);
  Main::input=&Main::window->input;
  Main::render=&Main::window->render;
  Main::updater=new Updater();
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
  //delete Main::updater;
  //Main::updater=nullptr;
  //delete Main::window;
  //Main::window=nullptr;
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

//https://stackoverflow.com/questions/3596781/how-to-detect-if-the-current-process-is-being-run-by-gdb
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
bool debuggerIsAttached()
{
    char buf[4096];

    const int status_fd = open("/proc/self/status", O_RDONLY);
    if (status_fd == -1)
        return false;

    const ssize_t num_read = read(status_fd, buf, sizeof(buf) - 1);
    close(status_fd);

    if (num_read <= 0)
        return false;

    buf[num_read] = '\0';
    constexpr char tracerPidString[] = "TracerPid:";
    const auto tracer_pid_ptr = strstr(buf, tracerPidString);
    if (!tracer_pid_ptr)
        return false;

    for (const char* characterPtr = tracer_pid_ptr + sizeof(tracerPidString) - 1; characterPtr <= buf + num_read; ++characterPtr)
    {
        if (isspace(*characterPtr))
            continue;
        else
            return isdigit(*characterPtr) != 0 && *characterPtr != '0';
    }

    return false;
}
