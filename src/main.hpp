#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void quit();

class Input;
class Render;
class Window;

struct Main{
  static Input* input;
  static Render* render;
  static Window* window;
};
