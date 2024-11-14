#pragma once
#include "util/collections.hpp"
#include "util/mat.hpp"
#include "Spatial.hpp"
#include "util/Cycle.hpp"
#include "Light.hpp"
#include "Shader.hpp"
#include "TaskPool.hpp"

class Render;
class Window;

struct Camera : public Spatial{
  virtual fmat4 getProjection(ivec2 vp_size) const=0;
  virtual fmat4 getView() const;
};

struct PerspectiveCamera : public Camera{
  float fov=PI/4;
  float near=0.01;
  float far=1000.0;
  fmat4 getProjection(ivec2 vp_size) const override;
};

struct OrthographicCamera : public Camera{
  float size=10.0;
  float near=0.01;
  float far=1000.0;
  fmat4 getProjection(ivec2 vp_size) const override;
};

struct Renderable{
  virtual void render() const = 0;
};


//class Render : public STATIC_CYCLE(&Renderable::render), public virtual MemSafe{
class Viewport{

  fmat4 current_projection;
  fmat4 current_view;
  ivec2 size=ivec2(1024,576);

  //List<SafePtr<Light>> lights;
  //UniformBuffer _light_buffer;

  //void preCycle();

  //TaskCycle render_tasks;

public:

  std::list<std::weak_ptr<Renderable>> objects;

  const fmat4& projection=current_projection;
  const fmat4& view=current_view;
  //const UniformBuffer& light_buffer=_light_buffer;

  std::unique_ptr<Camera> camera;

  virtual void setSize(ivec2 to){size=to;}
  virtual ivec2 getSize() const {return size;}

  void addLight(Light& light);
  void removeLight(Light& light);

  Viewport();
};
/*
class WindowRender : public Render, public virtual MemSafe{
  void preRender();
  void postRender();
public:
  Window& window;
  WindowRender(Window& win);

  virtual void setSize(ivec2 to) override;
  virtual ivec2 getSize() const  override;
};
*/
