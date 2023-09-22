#include "Spatial.hpp"
#include <glm/gtc/matrix_transform.hpp>

inline void Spatial::translate(dvec3 by){
  transform=glm::translate(transform,by);
}

inline void Spatial::rotate(dvec3 axis, double angle){
  transform=glm::rotate(transform,angle,axis);
}

inline void Spatial::scale(double by){
  transform=glm::scale(transform,dvec3(by));
}

inline dvec3 Spatial::position() const{
  return transform[3];
}

inline void Spatial::position(dvec3 to){
  transform[3]=dvec4(to,1);
}

inline dvec3 Spatial::xAxis() const{
  return dvec3(transform[0][0],transform[1][0],transform[2][0]);
}
inline dvec3 Spatial::yAxis() const{
  return dvec3(transform[0][1],transform[1][1],transform[2][1]);
}
inline dvec3 Spatial::zAxis() const{
  return dvec3(transform[0][2],transform[1][2],transform[2][2]);
}

inline void Spatial::xAxis(dvec3 to){
  transform[0][0]=to.x;
  transform[1][0]=to.y;
  transform[2][0]=to.z;
}
inline void Spatial::yAxis(dvec3 to){
  transform[0][0]=to.x;
  transform[1][0]=to.y;
  transform[2][0]=to.z;
}
inline void Spatial::zAxis(dvec3 to){
  transform[0][0]=to.x;
  transform[1][0]=to.y;
  transform[2][0]=to.z;
}
