#pragma once
#include "Render.hpp"
#include "Input.hpp"

struct GLFWwindow;

class Window{
  static Map<GLFWwindow*,Window*> ptr_map;
  GLFWwindow* window=nullptr;

  static GLFWwindow* make_new_window();

public:

  Input input{*this};
  WindowRender render{*this};

  ivec2 getSize() const;
  void setSize(ivec2 to);

  GLFWwindow* glfw() const{return window;}
  static Window* fromGLFW(GLFWwindow*ptr);

  Window();
  Window(GLFWwindow*);

  friend void init();
};
