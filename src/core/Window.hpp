#pragma once
#include "Render.hpp"

struct GLFWwindow;

class Window{
  inline static GLFWwindow* window=nullptr;

  //static GLFWwindow* make_new_window();

public:

  //Input input{*this};
  //WindowRender render{*this};
  //static WindowRender& render();
  static Viewport& viewport();

  static ivec2 getSize();
  static void setSize(ivec2 to);

  static GLFWwindow* glfw() {return window;}

  static void init(GLFWwindow*);

  //Window();
  //Window(GLFWwindow*);

  //friend void init();
};
