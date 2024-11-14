#pragma once
#include "util/mat.hpp"
#include "Spatial.hpp"
#include "Light.hpp"
#include "Shader.hpp"
#include <memory>
#include <list>

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
  virtual void render(int mode) = 0;
};


//class Render : public STATIC_CYCLE(&Renderable::render), public virtual MemSafe{
class Viewport{

  fmat4 current_projection;
  fmat4 current_view;
  ivec2 size=ivec2(1024,576);

  std::list<std::shared_ptr<Light>> lights;
  UniformBuffer _light_buffer;

  //void preCycle();

  //TaskCycle render_tasks;

protected:

  virtual void preRender(){};
  virtual void postRender(){};

public:
  int mode = 0;

  std::list<std::weak_ptr<Renderable>> objects;

  const fmat4& projection=current_projection;
  const fmat4& view=current_view;
  const UniformBuffer& light_buffer=_light_buffer;

  std::shared_ptr<Camera> camera;

  void render();

  virtual void setSize(ivec2 to){size=to;}
  virtual ivec2 getSize() const {return size;}

  void addLight(std::shared_ptr<Light> light);
  void removeLight(std::shared_ptr<Light> light);

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
