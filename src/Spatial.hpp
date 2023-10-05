#pragma once
#include "util/glm.hpp"

struct Spatial{
  vec3 scale{1,1,1};
  mat3 rotation{1};
  vec3 position{0};

  mat4 getTransform() const;
  void rotate(vec3 axis, float theta);
};
