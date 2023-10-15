#pragma once
#include "Spatial.hpp"
#include "util/mem.hpp"

struct Light : public virtual Spatial, public virtual MemSafe{
  /*
   * DIRECTIONAL: direction is local -Z axis;
   * LINE: stretches from local origin to local (0,0,-1).
   * PLANE: stretches from -plane_size/2 to plane_size/2 in local space.
   */
  enum Type:int{VOID=0,DIRECTIONAL=1,POINT=2,LINE=3,PLANE=4};
  Type type=VOID;

  //parameter that determines energy and luminosity of light
  double lumin=1.0;

  //increases width of emmitting area
  //when <1, uses more energy conserving method
  //when >1, uses more luminosity conserving method
  double spread=1.0;

  fvec3 color{1,1,1};
  fvec2 plane_size{1,1};
};

STRUCT_TO_STRING(Light,type,lumin,spread,color,plane_size,transform);
