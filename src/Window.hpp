#pragma once
#include "main.hpp"
#include "Render.hpp"
#include "Input.hpp"

class Window{
  static Map<GLFWwindow*,Window*> ptrMap;
  GLFWwindow* window=nullptr;

public:

  Input input{*this};
  WindowRender render{*this};

  uvec2 getSize() const;
  void setSize(uvec2 to);

  GLFWwindow* glfw() const{return window;}
  static Window* fromGLFW(GLFWwindow*ptr);

  Window();
};
