#pragma once
#include "util/math.hpp"
#include "util/quat.hpp"
struct Spatial{
  fvec3 scale{1,1,1};
  fquat rotation=quatIdentity<float>();
  fvec3 position{0,0,0};

  fmat4 getTransform() const;
  void rotate(fvec3 axis, float theta);
};
