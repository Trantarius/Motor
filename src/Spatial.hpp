#pragma once
#include "util/math.hpp"

struct Transform{
  dvec3 position{0,0,0};
  dquat rotation=quatIdentity<double>();
  dvec3 scale{1,1,1};

  dmat4 toMatrix() const;
  dmat4 toInvMatrix() const;

  dvec3 operator*(const dvec3& v) const;

  Transform& translate(const dvec3& delta);
  Transform& rotate(const dvec3& axis,double theta);
  Transform& scaleBy(const dvec3& s);
  Transform& scaleBy(double s);
};

struct Spatial{
  Transform transform;

  dvec3& position=transform.position;
  dquat& rotation=transform.rotation;
  dvec3& scale=transform.scale;
};
