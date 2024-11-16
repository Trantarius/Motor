#include "Window.hpp"
#include "Render.hpp"
#include "defs/gl_defs.hpp"

struct WindowRender : public Viewport{
	void preRender() override;
	void postRender() override;
	void setSize(ivec2 to) override;
	ivec2 getSize() const override;
};

WindowRender* window_viewport = nullptr;

ivec2 Window::getSize(){
	int x,y;
	glfwGetWindowSize(window,&x,&y);
	return ivec2(x,y);
}

void Window::setSize(ivec2 to){
	glfwSetWindowSize(window,to.x,to.y);
}

void framebufferResizeCallback(GLFWwindow* window, int width, int height){
	glfwMakeContextCurrent(window);
	glViewport(0,0,width,height);
}

void Window::init(GLFWwindow* win){

	window=win;
	glfwSetFramebufferSizeCallback(win,framebufferResizeCallback);

	window_viewport = new WindowRender();
}

void WindowRender::preRender(){
	glfwMakeContextCurrent(Window::glfw());
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void WindowRender::postRender(){
	glfwSwapBuffers(Window::glfw());
}

void WindowRender::setSize(ivec2 to){
	Window::setSize(to);
}
ivec2 WindowRender::getSize() const {
	return Window::getSize();
}

Viewport& Window::viewport(){
	return *window_viewport;
}
