#pragma once
#include "util/collections.hpp"
#include "util/math.hpp"
#include "util/funcy.hpp"
#include "Spatial.hpp"
#include "util/mem.hpp"
#include "Cycle.hpp"
#include "Light.hpp"
#include "Shader.hpp"

class Render;
class Window;

struct Camera : public Spatial{
  Render* renderer=nullptr;
  virtual fmat4 getProjection(Render*) const=0;
  virtual fmat4 getView(Render*) const;
};

struct PerspectiveCamera : public Camera{
  float fov=PI/4;
  float near=0.01;
  float far=1000.0;
  fmat4 getProjection(Render*) const override;
};

struct OrthographicCamera : public Camera{
  float size=10.0;
  float near=0.01;
  float far=1000.0;
  fmat4 getProjection(Render*) const override;
};

class Renderable : public virtual MemSafe{
protected:
  virtual void render(Render*)=0;
public:
  friend class Render;
};


class Render : public STATIC_CYCLE(&Renderable::render), public virtual MemSafe{

  fmat4 current_projection;
  fmat4 current_view;
  ivec2 size=ivec2(1024,576);

  List<SafePtr<Light>> lights;
  UniformBuffer _light_buffer;

  void preCycle();
public:

  const fmat4& projection=current_projection;
  const fmat4& view=current_view;
  const UniformBuffer& light_buffer=_light_buffer;

  Unique<Camera> camera;

  virtual void setSize(ivec2 to){size=to;}
  virtual ivec2 getSize() const {return size;}

  void addLight(Light& light);
  void removeLight(Light& light);

  Render();
};

class WindowRender : public Render, public virtual MemSafe{
  void preRender();
  void postRender();
public:
  Window& window;
  WindowRender(Window& win);

  virtual void setSize(ivec2 to) override;
  virtual ivec2 getSize() const  override;
};
