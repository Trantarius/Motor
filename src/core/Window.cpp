#include "Window.hpp"
#include "Render.hpp"
#include "defs/gl_defs.hpp"
#include "Engine.hpp"


ivec2 Window::getSize() const{
	int x,y;
	glfwGetWindowSize(window,&x,&y);
	return ivec2(x,y);
}

void Window::setSize(ivec2 to){
	glfwSetWindowSize(window,to.x,to.y);
}

std::string Window::getTitle() const{
	return glfwGetWindowTitle(window);
}

void Window::setTitle(const std::string& title){
	glfwSetWindowTitle(window,title.c_str());
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height){
	assert(window_obj_from_glfw.contains(window));
	Window* winobj = window_obj_from_glfw[window];
	winobj->framebuffer_size=ivec2{width,height};
}

Window::Window(){

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	if(Engine::main_window)
		window = glfwCreateWindow(1024,600,"Hello",NULL,Engine::main_window->glfw());
	else
		window = glfwCreateWindow(1024,600,"Hello",NULL,NULL);
	assert(window);
	glfwSetFramebufferSizeCallback(window,Window::framebufferResizeCallback);
}

Window::~Window(){
	glfwDestroyWindow(window);
}

void Window::render(){
	assert(viewport);
	glfwMakeContextCurrent(window);
	viewport->setSize(framebuffer_size);
	viewport->render();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, viewport->getOutputFramebuffer());
	ivec2 src_size = viewport->getSize();
	ivec2 dst_size = framebuffer_size;
	glBlitFramebuffer(0, 0, src_size.x, src_size.y, 0, 0, dst_size.x, dst_size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glfwSwapBuffers(window);
}
