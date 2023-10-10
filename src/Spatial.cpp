#include "Spatial.hpp"

fmat4 Spatial::getTransform() const{
  fmat4 mat(
    scale.x,0,0,0,
    0,scale.y,0,0,
    0,0,scale.z,0,
    0,  0,  0,  1
  );
  mat=fmat4(quatMtx(rotation))*mat;

  fmat4 tf=fmat4(mat);
  tf[3]+=fvec4(position,0);
  return tf;
}

void Spatial::rotate(fvec3 axis,float theta){
  fquat rot=quat(axis,theta);
  rotation=quatMul(rot,rotation);
}
