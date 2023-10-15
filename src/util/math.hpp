#pragma once
#include "vec.hpp"
#include "mat.hpp"
#include "quat.hpp"
#include <cmath>

using std::min;
using std::max;

constexpr double PI=3.1415926535897932384626433832795028841971693993751058;

/*
template<typename T>
inline mat<T,3> rotationMtx(vec<T,3> axis,T theta){
  //https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
  T c=cos(theta);
  T s=sin(theta);
  mat<T,3> rot(
    c+axis.x*axis.x*(1-c),          axis.x*axis.y*(1-c)-axis.z*s,     axis.x*axis.z*(1-c)+axis.y*s,
    axis.y*axis.x*(1-c)+axis.z*s,   c+axis.y*axis.y*(1-c),            axis.y*axis.z*(1-c)-axis.x*s,
    axis.z*axis.x*(1-c)-axis.y*s,   axis.z*axis.y*(1-c)+axis.x*s,     c+axis.z*axis.z*(1-c)
  );
  return rot;
}

inline fmat4 translate(fmat4 tform,fvec3 delta){
  tform[0][3]+=delta.x;
  tform[1][3]+=delta.y;
  tform[2][3]+=delta.z;
  return tform;
}
*/
