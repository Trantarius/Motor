#pragma once
#include "vec.hpp"
#include "mat.hpp"
#include <cmath>

/**
 * treats a vec4 as a quaternion
 * stores real part as w
 * https://www.3dgep.com/understanding-quaternions/#Quaternions
 * https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
 */

typedef vec<float,4> fquat;
typedef vec<double,4> dquat;


#define QUAT(T) vec<T,4>

template<typename T>
QUAT(T) quat(const vec<T,3>& axis,T theta){
  QUAT(T) ret;
  ret.w=cos(theta/2);
  ret.xyz=axis*sin(theta/2);
  return ret;
}

template<typename T>
constexpr QUAT(T) quatIdentity(){
  return normalize(QUAT(T)(0,0,0,1));
}

template<typename A,typename B>
QUAT(decltype(A()*B())) quatMul (const QUAT(A)& a,const QUAT(B)& b){
  //https://en.wikipedia.org/wiki/Quaternion#Hamilton_product
  return QUAT(decltype(A()*B()))(
    a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
    a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
    a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w,
    a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z
  );
}

template<typename T>
QUAT(T) quatConj(const QUAT(T)& q){
  return QUAT(T)(-q.x,-q.y,-q.z,q.w);
}

template<typename T>
QUAT(T) quatInv(const QUAT(T)& q){
  return quatConj(q)/lensqr(q);
}

template<typename A,typename B>
#define R decltype(A()*B())
vec<R,3> quatRot(const QUAT(A)& q,const vec<B,3>& v){
  QUAT(R) p(v,0);
  QUAT(R) ret=quatMul(quatMul(q,p),quatInv(q));
  return ret.xyz;
}
#undef R

template<typename T>
mat<T,3> quatMtx(const QUAT(T)& q){
  //https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Conversion_to_and_from_the_matrix_representation
  T s=2.0/lensqr(q);
  T a=q.w, b=q.x, c=q.y,d=q.z;

  T bs = b*s;  T cs = c*s;  T ds = d*s;
  T ab = a*bs; T ac = a*cs; T ad = a*ds;
  T bb = b*bs; T bc = b*cs; T bd = b*ds;
  T cc = c*cs; T cd = c*ds; T dd = d*ds;

  return mat<T,3>(
    1-cc-dd,    bc-ad,    bd+ac,
    bc+ad,    1-bb-dd,    cd-ab,
    bd-ac,    cd+ab,    1-bb-cc
  );
}

#undef QUAT
