#pragma once
#include "Render.hpp"
#include "Updater.hpp"

class OrbitCamera : public Updatable, public PerspectiveCamera{
  fvec2 last_mouse_pos{};
public:
  float orbit_distance=5.0;
  float speed=0.01;
  void update(Updater* up) override;
  fmat4 getView(Render*) const override;
};
