#pragma once
#include "util/vec.hpp"
#include "Render.hpp"

struct GLFWwindow;

class Window{
	inline static std::map<GLFWwindow*,Window*> window_obj_from_glfw;
	GLFWwindow* window=nullptr;
	ivec2 framebuffer_size{1024,600};
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

public:

	std::unique_ptr<Viewport> viewport;

	ivec2 getSize() const;
	void setSize(ivec2 to);
	std::string getTitle() const;
	void setTitle(const std::string&);

	GLFWwindow* glfw() {return window;}
	void render();

	Window();
	~Window();

	Window(const Window&)=delete;
	Window(Window&&)=delete;
	Window& operator=(const Window&)=delete;
	Window& operator=(Window&&)=delete;
};
