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

	main_window = std::make_unique<Window>();

	glfwMakeContextCurrent(main_window->glfw());
	glfwSwapInterval(1);//enable Vsync
	if(glfwRawMouseMotionSupported()){
		glfwSetInputMode(main_window->glfw(),GLFW_RAW_MOUSE_MOTION,GLFW_TRUE);
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		throw std::runtime_error("Failed to initialize GLAD");
	}

	glDebugMessageCallback(openGLDebugCallback,NULL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	main_window->viewport = std::make_unique<Viewport>();

	Shader::loadAllShaderFiles("src/shaders");

	Input::init();
}

void Engine::mainLoop(){

	size_t frame_counter=0;
	double last_frame=Time::now();

	while(!is_quitting){
		glfwPollEvents();
		frame_cycle.dumpInto(main_thread);
		main_thread.flush();
		main_window->render();
		if(glfwWindowShouldClose(main_window->glfw())){
			quit();
		}

		frame_counter++;
		double now=Time::now();
		if(now-last_frame>1.0){
			if(print_fps)
				print(frame_counter);
			last_frame=now;
			frame_counter=0;
		}
	}
	is_quitting = false;
}

void Engine::terminate(){
	main_thread.flush();
	main_window.reset();
	glfwTerminate();
}

void Engine::quit(){
	is_quitting = true;
}
