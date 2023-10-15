#pragma once
#include "core/Render.hpp"
#include "core/Updater.hpp"

class OrbitCamera : public Updatable, public PerspectiveCamera{
  void onEscapePress();
public:
  float orbit_distance=5.0;
  float speed=0.01;
  void update(Updater* up) override;
  fmat4 getView(Render*) const override;
  OrbitCamera();
  ~OrbitCamera();
};
