#pragma once
#include "core/Mesh.hpp"
#include "core/Updater.hpp"
#include "core/Light.hpp"

class OrbitLight : public Updatable , public Light{
  void update(Updater*) override;
  Mesh mesh;

public:
  OrbitLight();

  struct{
    int a,b,c;
  }keys;
};
