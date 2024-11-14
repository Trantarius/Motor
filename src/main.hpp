#pragma once
#include "defs/gl_defs.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void quit();

class Input;
class Render;
class Window;
class Updater;

struct Main{
  inline static Input* input = nullptr;
  inline static Render* render = nullptr;
  inline static Window* window = nullptr;
  inline static Updater* updater = nullptr;
};
