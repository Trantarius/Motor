#include "Engine.hpp"
#include "Input.hpp"
#include "Render.hpp"
#include "Window.hpp"
#include "Updater.hpp"
#include "util/print.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

void Engine::init(){
	if(!glfwInit()){
		throw std::runtime_error("glfwInit failed");
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
		throw std::runtime_error("Failed to initialize GLAD");
	}

	glDebugMessageCallback(openGLDebugCallback,NULL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	Shader::loadAllShaderFiles("src/shaders");

	window=new Window(winptr);
	input=&window->input;
	render=&window->render;
	updater=new Updater();
}

void Engine::main_loop(){

  size_t frame_counter=0;
  double last_frame=time();

  while(!is_quitting){
    glfwPollEvents();
    input->update();
    updater->cycle(updater);
    render->cycle(render);
    if(glfwWindowShouldClose(window->glfw())){
      quit();
    }

    frame_counter++;
    double now=time();
    if(now-last_frame>1.0){
      last_frame=now;
      frame_counter=0;
    }
  }
  glfwTerminate();
}

void Engine::quit(){
	is_quitting = true;
}
