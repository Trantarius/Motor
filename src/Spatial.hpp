#pragma once
#include "util/glm.hpp"

struct Spatial{
  dmat4 transform{1};

   void translate(dvec3 by);
   void rotate(dvec3 axis, double angle);
   void scale(double by);

   dvec3 position() const;
   void position(dvec3 to);

   dvec3 xAxis() const;
   dvec3 yAxis() const;
   dvec3 zAxis() const;

   void xAxis(dvec3 to);
   void yAxis(dvec3 to);
   void zAxis(dvec3 to);
};
