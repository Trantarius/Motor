#pragma once
#include "util/math.hpp"
struct Spatial{
  fvec3 scale{1,1,1};
  fmat3 rotation=fmat3::identity();
  fvec3 position{0,0,0};

  fmat4 getTransform() const;
  void rotate(fvec3 axis, float theta);
};
