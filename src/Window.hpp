#pragma once
#include "main.hpp"
#include "Render.hpp"
#include "Input.hpp"

class Window{
  static Map<GLFWwindow*,Window*> ptr_map;
  GLFWwindow* window=nullptr;

public:

  Input input{*this};
  WindowRender render{*this};

  ivec2 getSize() const;
  void setSize(ivec2 to);

  GLFWwindow* glfw() const{return window;}
  static Window* fromGLFW(GLFWwindow*ptr);

  Window();
};
