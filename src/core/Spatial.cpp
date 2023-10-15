#include "Spatial.hpp"


Transform Transform::inverse() const{
  Transform b;
  b.scale=1/scale;
  b.rotation=quatConj(rotation);
  b.position=-quatRot(b.rotation,position*b.scale);
  return b;
}

dvec3 Transform::operator*(const dvec3& v) const {
  return quatRot(rotation,scale*v)+position;
}

Transform Transform::operator*(const Transform& b)const{
  Transform ret=b;
  ret.scale*=scale;
  ret.position*=scale;
  ret.rotation=quatMul(rotation,ret.rotation);
  ret.position=quatRot(rotation,ret.position);
  return ret;
}

Transform Transform::operator/(const Transform& b) const{
  return b.inverse()**this;
}

dvec3 operator/(const dvec3& v,const Transform& t){
  return quatRot(quatConj(t.rotation),v-t.position)/t.scale;
}

Transform& Transform::translate(const dvec3& delta){
  position+=delta;
  return *this;
}

Transform& Transform::rotate(const dvec3& axis,double theta){
  dquat rot=quat(axis,theta);
  rotation=quatMul(rot,rotation);
  return *this;
}

Transform& Transform::scaleBy(double s){
  scale*=s;
  return *this;
}

dmat4 Transform::toMatrix() const{
  dmat4 mat(
    scale,0,0,0,
    0,scale,0,0,
    0,0,scale,0,
    0,  0,  0,  1
  );
  mat=dmat4(quatMtx(rotation))*mat;

  mat[0][3]=position.x;
  mat[1][3]=position.y;
  mat[2][3]=position.z;
  return mat;
}

dmat4 Transform::toInvMatrix() const{
  dmat4 mtx=toMatrix();
  dmat4 ret;

  double* M=(double*)&mtx;
  double* out=(double*)&ret;

  const double x0 = 1.0/M[0];
  const double x1 = x0*M[8];
  const double x2 = -x1*M[1] + M[9];
  const double x3 = x0*M[4];
  const double x4 = 1.0/(-x3*M[1] + M[5]);
  const double x5 = x0*x2*x4*M[4] - x1;
  const double x6 = -x3*M[2] + M[6];
  const double x7 = x2*x4;
  const double x8 = x6*x7;
  const double x9 = 1.0/(-x1*M[2] - x8 + M[10]);
  const double x10 = x9*M[2];
  const double x11 = x5*x9;
  const double x12 = x4*(-x11*x6 - x3);
  const double x13 = x4*(x8*x9 + 1);
  const double x14 = -x3*M[3] + M[7];
  const double x15 = x1*M[3] + x14*x7 - M[11];
  const double x16 = x15*x9;
  const double x17 = x4*(-x14 - x16*x6);

  out[0] = x0*(-x10*x5 - x12*M[1] + 1);
  out[1] = x0*(-x13*M[1] + x2*x4*x9*M[2]);
  out[2] = x0*(-x10 + x4*x6*x9*M[1]);
  out[3] = x0*(-x10*x15 - x17*M[1] - M[3]);
  out[4] = x12;
  out[5] = x13;
  out[6] = -x4*x6*x9;
  out[7] = x17;
  out[8] = x11;
  out[9] = -x7*x9;
  out[10] = x9;
  out[11] = x16;
  out[12] = 0;
  out[13] = 0;
  out[14] = 0;
  out[15] = 1;

  return ret;
}
