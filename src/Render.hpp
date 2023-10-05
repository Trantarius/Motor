#pragma once
#include "util/collections.hpp"
#include "util/glm.hpp"
#include "util/funcy.hpp"
#include "Spatial.hpp"
#include <memory>

class Render;

struct Camera : public Spatial{
  Render* renderer=nullptr;
  virtual mat4 getProjection()=0;
};

struct PerspectiveCamera : public Camera{
  float fov=PI/4;
  float near=0.01;
  float far=1000.0;
  mat4 getProjection() override;
};

struct OrthographicCamera : public Camera{
  float size=10.0;
  float near=0.01;
  float far=1000.0;
  mat4 getProjection() override;
};

class Render{

  mat4 current_projection;
  mat4 current_view;

public:
  static Render main;

  ivec2 size=ivec2(800,600);
  std::unique_ptr<Camera> camera;
  List<Call> render_calls;
  void render();

  mat4 getProjection(){return current_projection;}
  mat4 getView(){return current_view;}
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


