#pragma once
#include "util/collections.hpp"
#include "util/glm.hpp"
#include "util/funcy.hpp"
#include "Spatial.hpp"
#include "util/mem.hpp"

class Render;
class Window;

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

class Renderable : public MemSafe{
protected:
  virtual void render()=0;
public:
  friend class Render;
};


class Render{

  mat4 current_projection;
  mat4 current_view;
  uvec2 size=uvec2(1024,576);

protected:
  static Render* _current;
  List<SafePtr<Renderable>> render_calls;

public:

  Unique<Camera> camera;

  virtual void pre_render()=0;
  virtual void post_render()=0;

  void render();

  mat4 getProjection(){return current_projection;}
  mat4 getView(){return current_view;}
  static Render& current(){assert(_current!=nullptr);return *_current;}

  virtual void setSize(uvec2 to){size=to;}
  virtual uvec2 getSize() const {return size;}

  void add(Renderable* rable);
  void remove(Renderable* rable);
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
