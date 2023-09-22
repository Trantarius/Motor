#pragma once
#include "util/collections.hpp"
#include "util/glm.hpp"
#include "util/funcy.hpp"
#include "Spatial.hpp"

struct Camera : public Spatial{
  mat4 projection{1};
};

class Render{
public:
  static Render main;
  Camera camera;
  List<Call> render_calls;
  void render();
};

class Renderable : public Spatial{
  List<Call>::iterator renderables_pos;
protected:
  virtual void render()=0;
public:
  Render* renderer=&Render::main;

  Renderable(){
    renderer->render_calls.push_back(MemFunc(this,render));
    renderables_pos=--renderer->render_calls.end();
  }
  ~Renderable(){
    renderer->render_calls.erase(renderables_pos);
  }
  friend class Render;
};


