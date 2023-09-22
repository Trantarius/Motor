#pragma once
#include "util/glm.hpp"

struct Spatial{
  dmat4 transform{1};

  inline void translate(dvec3 by);
  inline void rotate(dvec3 axis, double angle);
  inline void scale(double by);

  inline dvec3 position() const;
  inline void position(dvec3 to);

  inline dvec3 xAxis() const;
  inline dvec3 yAxis() const;
  inline dvec3 zAxis() const;

  inline void xAxis(dvec3 to);
  inline void yAxis(dvec3 to);
  inline void zAxis(dvec3 to);
};
