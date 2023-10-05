#include "Spatial.hpp"
#include <glm/gtc/matrix_transform.hpp>

mat4 Spatial::getTransform() const{
  mat3 mat(
    scale.x,0,0,
    0,scale.y,0,
    0,0,scale.z
  );
  mat=rotation*mat;

  mat4 tf=mat4(mat);
  tf[3]+=vec4(position,0);
  return tf;
}

void Spatial::rotate(vec3 axis,float theta){
  //https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
  float c=cos(theta);
  float s=sin(theta);
  mat3 rot(
    c+axis.x*axis.x*(1-c),          axis.x*axis.y*(1-c)-axis.z*s,     axis.x*axis.z*(1-c)+axis.y*s,
    axis.y*axis.x*(1-c)+axis.z*s,   c+axis.y*axis.y*(1-c),            axis.y*axis.z*(1-c)-axis.x*s,
    axis.z*axis.x*(1-c)-axis.y*s,   axis.z*axis.y*(1-c)+axis.x*s,     c+axis.z*axis.z*(1-c)
  );
  rot=transpose(rot);
  rotation=rotation*rot;
}
