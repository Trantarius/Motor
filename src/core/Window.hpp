#pragma once
#include "Render.hpp"

struct GLFWwindow;

class Window{
	inline static GLFWwindow* window=nullptr;

public:
	static Viewport& viewport();

	static ivec2 getSize();
	static void setSize(ivec2 to);

	static GLFWwindow* glfw() {return window;}

	static void init(GLFWwindow*);
};
