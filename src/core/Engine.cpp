#include "Engine.hpp"
#include "Input.hpp"
#include "Render.hpp"
#include "Window.hpp"
#include "util/print.hpp"
#include "util/time.hpp"

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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWwindow* win = glfwCreateWindow(1024,600,"Hello",NULL,NULL);
	assert(win);

	glfwMakeContextCurrent(win);
	glfwSwapInterval(1);//enable Vsync
	if(glfwRawMouseMotionSupported()){
		glfwSetInputMode(win,GLFW_RAW_MOUSE_MOTION,GLFW_TRUE);
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


	Window::init(win);
	Input::init();
}

void Engine::mainLoop(){

	size_t frame_counter=0;
	double last_frame=Time::now();

	while(!is_quitting){
		glfwPollEvents();
		Input::update();
		frame_cycle.flush();
		frame_cycle.recycle();
		Window::viewport().render();
		if(glfwWindowShouldClose(Window::glfw())){
			quit();
		}

		frame_counter++;
		double now=Time::now();
		Engine::dT = now-last_frame;
		if(now-last_frame>1.0){
			last_frame=now;
			frame_counter=0;
		}
	}
}

void Engine::terminate(){
	frame_cycle.flush();
	glfwTerminate();
}

void Engine::quit(){
	is_quitting = true;
}
