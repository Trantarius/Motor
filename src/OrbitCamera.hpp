#pragma once
#include "Render.hpp"
#include "Updater.hpp"

class OrbitCamera : public Updatable, public PerspectiveCamera{
  vec2 last_mouse_pos{};
public:
  float orbit_distance=5.0;
  float speed=0.01;
  void update(Updater* up) override;
  mat4 getView() const override;
};
