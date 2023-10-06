#pragma once
#include "util/collections.hpp"
#include "util/math.hpp"
#include "util/funcy.hpp"
#include "Spatial.hpp"
#include "util/mem.hpp"
#include "Cycle.hpp"

class Render;
class Window;

struct Camera : public Spatial{
  Render* renderer=nullptr;
  virtual fmat4 getProjection() const=0;
  virtual fmat4 getView() const;
};

struct PerspectiveCamera : public Camera{
  float fov=PI/4;
  float near=0.01;
  float far=1000.0;
  fmat4 getProjection() const override;
};

struct OrthographicCamera : public Camera{
  float size=10.0;
  float near=0.01;
  float far=1000.0;
  fmat4 getProjection() const override;
};

class Renderable : public MemSafe{
protected:
  virtual void render(Render*)=0;
public:
  friend class Render;
};


class Render : public CYCLE(Renderable::render){

  fmat4 current_projection;
  fmat4 current_view;
  ivec2 size=ivec2(1024,576);

public:

  Unique<Camera> camera;

  virtual void pre_render()=0;
  virtual void post_render()=0;

  void pre_cycle() override;
  void post_cycle() override;

  fmat4 getProjection(){return current_projection;}
  fmat4 getView(){return current_view;}

  virtual void setSize(ivec2 to){size=to;}
  virtual ivec2 getSize() const {return size;}
};

class WindowRender : public Render{
  Window& window;
public:
  WindowRender(Window& win):window(win){}

  virtual void pre_render() override;
  virtual void post_render() override;

  virtual void setSize(ivec2 to) override;
  virtual ivec2 getSize() const  override;

  Window& get_window() const{return window;}
};
