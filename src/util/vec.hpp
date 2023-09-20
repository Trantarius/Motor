#pragma once
#include <type_traits>
#include "strings.hpp"

#define VEC_CLASS(N) \
T& operator[](int n){\
  return data[n];\
}\
const T& operator[](int n) const {\
  return data[n];\
}\
operator std::string(){\
  std::string str="["+tostr(data[0]);\
  for(int n=1;n<N;n++){\
    str+=", "+tostr(data[n]);\
  }\
  return str+"]";\
}\
template<typename _T>\
operator vec<_T,N>(){\
  vec<_T,N> v;\
  for(int n=0;n<N;n++){\
    v[n]=(_T)data[n];\
  }\
  return v;\
}\
template<typename...Ts> requires (std::is_convertible<Ts,T>::value&&...) && (sizeof...(Ts)==0 || sizeof...(Ts)==N)\
vec(Ts...args):data{static_cast<T>(args)...}{}

template<typename T,int N> requires (N>1)
struct vec{
  T data[N]{};

  VEC_CLASS(N)
};

template<typename T>
struct vec<T,2>{
  union{
    T data[2]{};
    struct{
      T x,y;
    };
  };

  VEC_CLASS(2)
};

template<typename T>
struct vec<T,3>{
  union{
    T data[3]{};
    struct{
      T x,y,z;
    };
  };

  VEC_CLASS(3)
};

template<typename T>
struct vec<T,4>{
  union{
    T data[4]{};
    struct{
      T x,y,z,w;
    };
  };

  VEC_CLASS(4)
};

#undef VEC_CLASS


typedef vec<float,2> fvec2;
typedef vec<float,3> fvec3;
typedef vec<float,4> fvec4;

typedef vec<double,2> dvec2;
typedef vec<double,3> dvec3;
typedef vec<double,4> dvec4;

typedef vec<int,2> ivec2;
typedef vec<int,3> ivec3;
typedef vec<int,4> ivec4;




#define VEC_BI_OP(OP)\
template<typename A,typename B,int N>\
inline vec<decltype(A() OP B()),N> operator OP (vec<A,N> a,vec<B,N> b){\
  vec<decltype(A() OP B()),N> ret;\
  for(int n=0;n<N;n++){\
    ret[n]=a[n] OP b[n];\
  }\
  return ret;\
}\
template<typename A,typename B,int N>\
inline vec<decltype(A() OP B()),N> operator OP (vec<A,N> a,B b){\
  vec<decltype(A() OP B()),N> ret;\
  for(int n=0;n<N;n++){\
    ret[n]=a[n] OP b;\
  }\
  return ret;\
}\
template<typename A,typename B,int N>\
inline vec<decltype(A() OP B()),N> operator OP (A a,vec<B,N> b){\
  vec<decltype(A() OP B()),N> ret;\
  for(int n=0;n<N;n++){\
    ret[n]=a OP b[n];\
  }\
  return ret;\
}\
template<typename T,int N>\
inline void operator OP##= (vec<T,N>& a,vec<T,N> b){\
  for(int n=0;n<N;n++){\
    a[n] OP##= b[n];\
  }\
}\
template<typename T,int N>\
inline void operator OP##= (vec<T,N>& a,T b){\
  for(int n=0;n<N;n++){\
    a[n] OP##= b;\
  }\
}

VEC_BI_OP(+)
VEC_BI_OP(-)
VEC_BI_OP(*)
VEC_BI_OP(/)
VEC_BI_OP(%)
VEC_BI_OP(&)
VEC_BI_OP(|)
VEC_BI_OP(^)

#undef VEC_BI_OP

#define VEC_UN_OP(OP)\
template<typename T,int N>\
inline vec<T,N> operator OP (vec<T,N> a){\
  vec<T,N> ret;\
  for(int n=0;n<N;n++){\
    ret[n]= OP a[n];\
  }\
  return ret;\
}

VEC_UN_OP(-)
VEC_UN_OP(~)

#undef VEC_UN_OP

template<typename T,int N>
T dot(vec<T,N> a,vec<T,N> b){
  T sum=0;
  for(int n=0;n<N;n++){
    sum+=a[n]*b[n];
  }
  return sum;
}

template<typename T,int N>
T lensqr(vec<T,N> v){
  return dot(v,v);
}

template<typename T,int N>
T len(vec<T,N> v){
  return sqrt(lensqr(v));
}

template<typename T>
vec<T,3> cross(vec<T,3> a,vec<T,3> b){
  return vec<T,3>(a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]);
}

template<typename T,int N>
vec<T,N> normalize(vec<T,N> v){
  return v/len(v);
}
