#pragma once
#include "strings.hpp"
#include "bits.hpp"
#include <cstring>
#include <cmath>

#define VEC_CLASS(N) \
T& operator[](int n){\
  return data[n];\
}\
const T& operator[](int n) const {\
  return data[n];\
}\
template<typename...Ts> requires (std::is_convertible<Ts,T>::value&&...) && (sizeof...(Ts)==0 || sizeof...(Ts)==N)\
vec(Ts...args):data{((T)args)...}{}\
\
template<int _N,typename...Ts> requires (std::is_convertible<Ts,T>::value&&...) && (N-_N==sizeof...(Ts))\
vec(vec<T,_N> v,Ts...args){\
  memcpy(data,v.data,_N*sizeof(T));\
  T arr[sizeof...(Ts)]{((T)args)...};\
  memcpy(data+_N,arr,sizeof...(Ts)*sizeof(T));\
}\
\
template<typename _T> requires std::is_convertible<_T,T>::value \
vec(const vec<_T,N>& v){\
  for(int n=0;n<N;n++){\
    data[n]=(T)v.data[n];\
  }\
}\
template<typename _T> requires std::is_convertible<_T,T>::value \
void operator=(const vec<_T,N>& b){\
  for(int n=0;n<N;n++){\
    data[n]=(T)b.data[n];\
  }\
}


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
    struct{
      vec<T,2> xy;
    };
    struct{
      T __x;
      vec<T,2> yz;
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
    struct{
      vec<T,2> xy,zw;
    };
    struct{
      T _x;
      vec<T,2> yz;
    };
    struct{
      vec<T,3> xyz;
    };
    struct{
      T __x;
      vec<T,3> yzw;
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
inline vec<decltype(A() OP B()),N> operator OP (const vec<A,N>& a,const vec<B,N>& b){\
  vec<decltype(A() OP B()),N> ret;\
  for(int n=0;n<N;n++){\
    ret[n]=a[n] OP b[n];\
  }\
  return ret;\
}\
template<typename A,typename B,int N>\
inline vec<decltype(A() OP B()),N> operator OP (const vec<A,N>& a,B b){\
  vec<decltype(A() OP B()),N> ret;\
  for(int n=0;n<N;n++){\
    ret[n]=a[n] OP b;\
  }\
  return ret;\
}\
template<typename A,typename B,int N>\
inline vec<decltype(A() OP B()),N> operator OP (A a,const vec<B,N>& b){\
  vec<decltype(A() OP B()),N> ret;\
  for(int n=0;n<N;n++){\
    ret[n]=a OP b[n];\
  }\
  return ret;\
}\
template<typename T,int N>\
inline void operator OP##= (vec<T,N>& a,const vec<T,N>& b){\
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
inline vec<T,N> operator OP (const vec<T,N>& a){\
  vec<T,N> ret;\
  for(int n=0;n<N;n++){\
    ret[n]= OP a[n];\
  }\
  return ret;\
}

VEC_UN_OP(-)
VEC_UN_OP(~)

#undef VEC_UN_OP

template<Printable T,int N>
string tostr(const vec<T,N>& v){
  std::string str="["+tostr(v.data[0]);
  for(int n=1;n<N;n++){
    str+=", "+tostr(v.data[n]);
  }
  return str+"]";
}

inline uint64_t hash(fvec2 v){
  return float_bits(v.x)<<32 ^ float_bits(v.y);
}
inline uint64_t hash(fvec3 v){
  return float_bits(v.x)<<32 ^ float_bits(v.y)<<16 ^ float_bits(v.z);
}
inline uint64_t hash(fvec4 v){
  return float_bits(v.x)<<32 ^ float_bits(v.y)<<22 ^ float_bits(v.z)<<11 ^ float_bits(v.w);
}

inline uint64_t hash(dvec2 v){
  return bitroll(float_bits(v.x),32) ^ float_bits(v.y);
}
inline uint64_t hash(dvec3 v){
  return bitroll(float_bits(v.x),40) ^ bitroll(float_bits(v.y),20) ^ float_bits(v.z);
}
inline uint64_t hash(dvec4 v){
  return bitroll(float_bits(v.x),48) ^ bitroll(float_bits(v.y),32) ^ bitroll(float_bits(v.z),16) ^ float_bits(v.w);
}

inline uint64_t hash(ivec2 v){
  return ((int64_t)v.x)<<32 ^ ((int64_t)v.y);
}
inline uint64_t hash(ivec3 v){
  return ((int64_t)v.x)<<32 ^ ((int64_t)v.y)<<16 ^ ((int64_t)v.z);
}
inline uint64_t hash(ivec4 v){
  return ((int64_t)v.x)<<32 ^ ((int64_t)v.y)<<22 ^ ((int64_t)v.z)<<11 ^ ((int64_t)v.w);
}

inline bool compare(fvec2 a,fvec2 b){
  return hash(a)<hash(b);
}
inline bool compare(fvec3 a,fvec3 b){
  return hash(a)<hash(b);
}
inline bool compare(fvec4 a,fvec4 b){
  return hash(a)<hash(b);
}

inline bool compare(dvec2 a,dvec2 b){
  return hash(a)<hash(b);
}
inline bool compare(dvec3 a,dvec3 b){
  return hash(a)<hash(b);
}
inline bool compare(dvec4 a,dvec4 b){
  return hash(a)<hash(b);
}

inline bool compare(ivec2 a,ivec2 b){
  return hash(a)<hash(b);
}
inline bool compare(ivec3 a,ivec3 b){
  return hash(a)<hash(b);
}
inline bool compare(ivec4 a,ivec4 b){
  return hash(a)<hash(b);
}



template<NumberType A,NumberType B,int N>
decltype(A()*B()) dot(vec<A,N> a,vec<B,N> b){
  decltype(A()*B()) sum=0;
  for(int n=0;n<N;n++){
    sum+=a[n]*b[n];
  }
  return sum;
}

template<NumberType T,int N>
T lensqr(vec<T,N> v){
  return dot(v,v);
}

template<NumberType T,int N>
T len(vec<T,N> v){
  return sqrt(lensqr(v));
}

template<NumberType A,NumberType B>
vec<decltype(A()*B()),3> cross(vec<A,3> a,vec<B,3> b){
  return vec<decltype(A()*B()),3>(a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]);
}

template<NumberType T,int N>
vec<T,N> normalize(vec<T,N> v){
  return v/len(v);
}
