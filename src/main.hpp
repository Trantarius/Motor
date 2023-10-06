#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void quit();
double time();

class Input;
class Render;
class Window;
class Updater;

struct Main{
  static Input* input;
  static Render* render;
  static Window* window;
  static Updater* updater;
};
