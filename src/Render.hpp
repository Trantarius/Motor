#pragma once
#include "util/collections.hpp"
#include "util/glm.hpp"
#include "util/funcy.hpp"
#include "Spatial.hpp"
#include "util/mem.hpp"
#include "Cycle.hpp"

class Render;
class Window;

struct Camera : public Spatial{
  Render* renderer=nullptr;
  virtual mat4 getProjection() const=0;
  virtual mat4 getView() const;
};

struct PerspectiveCamera : public Camera{
  float fov=PI/4;
  float near=0.01;
  float far=1000.0;
  mat4 getProjection() const override;
};

struct OrthographicCamera : public Camera{
  float size=10.0;
  float near=0.01;
  float far=1000.0;
  mat4 getProjection() const override;
};

class Renderable : public MemSafe{
protected:
  virtual void render(Render*)=0;
public:
  friend class Render;
};


class Render : public CYCLE(Renderable::render){

  mat4 current_projection;
  mat4 current_view;
  uvec2 size=uvec2(1024,576);

public:

  Unique<Camera> camera;

  virtual void pre_render()=0;
  virtual void post_render()=0;

  void pre_cycle() override;
  void post_cycle() override;

  mat4 getProjection(){return current_projection;}
  mat4 getView(){return current_view;}

  virtual void setSize(uvec2 to){size=to;}
  virtual uvec2 getSize() const {return size;}
};

class WindowRender : public Render{
  Window& window;
public:
  WindowRender(Window& win):window(win){}

  virtual void pre_render() override;
  virtual void post_render() override;

  virtual void setSize(uvec2 to) override;
  virtual uvec2 getSize() const  override;

  Window& get_window() const{return window;}
};
