#pragma once
#include <string>
#include <cmath>
#include "strings.hpp"

template<typename T,size_t N>
struct vec;

template<typename T>
struct vec<T,2>{
  union{
    T arr[2];
    struct{
      T x,y;
    };
  };

  T& operator[](size_t i){
    return arr[i];
  }
  const T& operator[](size_t i) const{
    return arr[i];
  }

  explicit constexpr vec(T t):arr{t,t}{}
  constexpr vec(T x,T y):arr{x,y}{}
  template<typename B>
  constexpr vec(const vec<B,2>& b): x(b.x), y(b.y){}


  struct Hash{
    size_t operator()(const vec& v) const{
      return std::hash<T>()(v.x) ^
        (std::hash<T>()(v.y)<<(sizeof(size_t)*4)) ^
        (std::hash<T>()(v.y)>>(sizeof(size_t)*4));
    }
  };

  constexpr vec()=default;
  constexpr vec(const vec&)=default;
  constexpr vec(vec&&)=default;
  constexpr vec& operator=(const vec&)=default;
  constexpr vec& operator=(vec&&)=default;

  constexpr auto operator<=>(const vec& v) const{
    if(x<=>v.x!=0)
      return x<=>v.x;
    return y<=>v.y;
  };
  constexpr bool operator==(const vec&) const = default;
  constexpr bool operator!=(const vec&) const = default;
  constexpr bool operator<(const vec&) const = default;
  constexpr bool operator>(const vec&) const = default;
  constexpr bool operator<=(const vec&) const = default;
  constexpr bool operator>=(const vec&) const = default;

#define AOP(OP) \
  template<typename B> \
  constexpr vec& operator OP##=(const B& t){ \
    *this = *this OP t; \
    return *this; \
  }

  AOP(%);
  AOP(^);
  AOP(&);
  AOP(|);
  AOP(+);
  AOP(-);
  AOP(*);
  AOP(/);
#undef AOP

};

#define UOP(OP) \
template<typename T> \
constexpr vec<decltype(OP T()),2> operator OP (const vec<T,2>& a){ \
  return {OP a.x, OP a.y}; \
}

UOP(~);
UOP(!);
UOP(-);
#undef UOP

#define BOP(OP) \
template<typename A, typename B> \
constexpr vec<decltype(A() OP B()),2> operator OP (const vec<A,2>& a, const vec<B,2>& b){ \
  return {a.x OP b.x, a.y OP b.y}; \
} \
template<typename A, typename B> \
constexpr vec<decltype(A() OP B()),2> operator OP (const vec<A,2>& a, const B& b){ \
  return {a.x OP b, a.y OP b}; \
} \
template<typename A, typename B> \
constexpr vec<decltype(A() OP B()),2> operator OP (const A& a, const vec<B,2>& b){ \
  return {a OP b.x, a OP b.y}; \
}

BOP(%);
BOP(^);
BOP(&);
BOP(|);
BOP(&&);
BOP(||);
BOP(+);
BOP(-);
BOP(*);
BOP(/);
#undef BOP





template<typename T>
struct vec<T,3>{
  union{
    T arr[3];
    struct{
      T x,y,z;
    };
  };

  T& operator[](size_t i){
    return arr[i];
  }
  const T& operator[](size_t i) const{
    return arr[i];
  }

  explicit constexpr vec(T t):arr{t,t,t}{}
  constexpr vec(T x,T y,T z):arr{x,y,z}{}
  constexpr vec(const vec<T,2>& xy, T z): arr{xy.x, xy.y, z} {}
  constexpr vec(T x, const vec<T,2>& yz): arr{x, yz[0], yz[1]} {}
  template<typename B>
  constexpr vec(const vec<B,3>& b): x(b.x), y(b.y), z(b.z){}


  struct Hash{
    size_t operator()(const vec& v) const{
      return std::hash<T>()(v.x) ^
        (std::hash<T>()(v.y)<<(sizeof(size_t)*8/3)) ^
        (std::hash<T>()(v.y)>>(sizeof(size_t)*8 - sizeof(size_t)*8/3)) ^
        (std::hash<T>()(v.z)<<(sizeof(size_t)*8 - sizeof(size_t)*8/3)) ^
        (std::hash<T>()(v.z)>>(sizeof(size_t)*8/3));
    }
  };

  constexpr vec()=default;
  constexpr vec(const vec&)=default;
  constexpr vec(vec&&)=default;
  constexpr vec& operator=(const vec&)=default;
  constexpr vec& operator=(vec&&)=default;

  constexpr auto operator<=>(const vec& v) const{
    if(x<=>v.x!=0)
      return x<=>v.x;
    if(y<=>v.y!=0)
      return y<=>v.y;
    return z<=>v.z;
  };
  constexpr bool operator==(const vec&) const = default;
  constexpr bool operator!=(const vec&) const = default;
  constexpr bool operator<(const vec&) const = default;
  constexpr bool operator>(const vec&) const = default;
  constexpr bool operator<=(const vec&) const = default;
  constexpr bool operator>=(const vec&) const = default;

#define AOP(OP) \
  template<typename B> \
  constexpr vec& operator OP##=(const B& t){ \
    *this = *this OP t; \
    return *this; \
  }

  AOP(%);
  AOP(^);
  AOP(&);
  AOP(|);
  AOP(+);
  AOP(-);
  AOP(*);
  AOP(/);
#undef AOP

};

#define UOP(OP) \
template<typename T> \
constexpr vec<decltype(OP T()),3> operator OP (const vec<T,3>& a){ \
  return {OP a.x, OP a.y, OP a.z}; \
}

UOP(~);
UOP(!);
UOP(-);
#undef UOP

#define BOP(OP) \
template<typename A, typename B> \
constexpr vec<decltype(A() OP B()),3> operator OP (const vec<A,3>& a, const vec<B,3>& b){ \
  return {a.x OP b.x, a.y OP b.y, a.z OP b.z}; \
} \
template<typename A, typename B> \
constexpr vec<decltype(A() OP B()),3> operator OP (const vec<A,3>& a, const B& b){ \
  return {a.x OP b, a.y OP b, a.z OP b}; \
} \
template<typename A, typename B> \
constexpr vec<decltype(A() OP B()),3> operator OP (const A& a, const vec<B,3>& b){ \
  return {a OP b.x, a OP b.y, a OP b.z}; \
}

BOP(%);
BOP(^);
BOP(&);
BOP(|);
BOP(&&);
BOP(||);
BOP(+);
BOP(-);
BOP(*);
BOP(/);
#undef BOP


template<typename T>
struct vec<T,4>{
  union{
    T arr[4];
    struct{
      T x,y,z,w;
    };
  };

  T& operator[](size_t i){
    return arr[i];
  }
  const T& operator[](size_t i) const{
    return arr[i];
  }

  explicit constexpr vec(T t):arr{t,t,t,t}{}
  constexpr vec(T x,T y,T z,T w):arr{x,y,z,w}{}
  constexpr vec(const vec<T,2>& xy, T z, T w): arr{xy.x, xy.y, z, w} {}
  constexpr vec(T x, const vec<T,2>& yz, T w): arr{x, yz[0], yz[1], w} {}
  constexpr vec(T x, T y, const vec<T,2>& zw): arr{x, y, zw[0], zw[1]} {}
  constexpr vec(const vec<T,3>& xyz, T w): arr{xyz.x, xyz.y, xyz.z, w} {}
  constexpr vec(T x, const vec<T,3>& yzw): arr{x, yzw[0], yzw[1], yzw[2]} {}
  template<typename B>
  constexpr vec(const vec<B,4>& b): x(b.x), y(b.y), z(b.z), w(b.w){}


  struct Hash{
    size_t operator()(const vec& v) const{
      return std::hash<T>()(v.x) ^
        (std::hash<T>()(v.y)<<(sizeof(size_t)*2)) ^
        (std::hash<T>()(v.y)>>(sizeof(size_t)*8 - sizeof(size_t)*2)) ^
        (std::hash<T>()(v.z)<<(sizeof(size_t)*4)) ^
        (std::hash<T>()(v.z)>>(sizeof(size_t)*4)) ^
        (std::hash<T>()(v.w)<<(sizeof(size_t)*8 - sizeof(size_t)*2)) ^
        (std::hash<T>()(v.w)>>(sizeof(size_t)*2));
    }
  };

  constexpr vec()=default;
  constexpr vec(const vec&)=default;
  constexpr vec(vec&&)=default;
  constexpr vec& operator=(const vec&)=default;
  constexpr vec& operator=(vec&&)=default;

  constexpr auto operator<=>(const vec& v) const{
    if(x<=>v.x!=0)
      return x<=>v.x;
    if(y<=>v.y!=0)
      return y<=>v.y;
    if(z<=>v.z!=0)
      return z<=>v.z;
    return w<=>v.w;
  };
  constexpr bool operator==(const vec&) const = default;
  constexpr bool operator!=(const vec&) const = default;
  constexpr bool operator<(const vec&) const = default;
  constexpr bool operator>(const vec&) const = default;
  constexpr bool operator<=(const vec&) const = default;
  constexpr bool operator>=(const vec&) const = default;

#define AOP(OP) \
  template<typename B> \
  constexpr vec& operator OP##=(const B& t){ \
    *this = *this OP t; \
    return *this; \
  }

  AOP(%);
  AOP(^);
  AOP(&);
  AOP(|);
  AOP(+);
  AOP(-);
  AOP(*);
  AOP(/);
#undef AOP

};

#define UOP(OP) \
template<typename T> \
constexpr vec<decltype(OP T()),4> operator OP (const vec<T,4>& a){ \
  return {OP a.x, OP a.y, OP a.z, OP a.w}; \
}

UOP(~);
UOP(!);
UOP(-);
#undef UOP

#define BOP(OP) \
template<typename A, typename B> \
constexpr vec<decltype(A() OP B()),4> operator OP (const vec<A,4>& a, const vec<B,4>& b){ \
  return {a.x OP b.x, a.y OP b.y, a.z OP b.z, a.w OP b.w}; \
} \
template<typename A, typename B> \
constexpr vec<decltype(A() OP B()),4> operator OP (const vec<A,4>& a, const B& b){ \
  return {a.x OP b, a.y OP b, a.z OP b, a.w OP b}; \
} \
template<typename A, typename B> \
constexpr vec<decltype(A() OP B()),4> operator OP (const A& a, const vec<B,4>& b){ \
  return {a OP b.x, a OP b.y, a OP b.z, a OP b.w}; \
}

BOP(%);
BOP(^);
BOP(&);
BOP(|);
BOP(&&);
BOP(||);
BOP(+);
BOP(-);
BOP(*);
BOP(/);
#undef BOP

template<typename T, size_t N> requires (N>4)
struct vec<T,N>{
  T arr[N];

  T& operator[](size_t i){
    return arr[i];
  }
  const T& operator[](size_t i) const{
    return arr[i];
  }

  template<typename...Ts>
  explicit constexpr vec(Ts...args): arr{std::forward(args)...} {}
  template<typename B>
  constexpr vec(const vec<B,N>& b){
    for(int n=0;n<N;n++){
      arr[n] = b[n];
    }
  }

  struct Hash{
    size_t operator()(const vec& v) const{
      size_t h = 0;
      for(int n=0;n<N;n++){
        h ^= (std::hash<T>()(v[n])<<n)^(std::hash<T>()(v[n])>>(sizeof(size_t)*8-n));
      }
      return h;
    }
  };

  constexpr vec()=default;
  constexpr vec(const vec&)=default;
  constexpr vec(vec&&)=default;
  constexpr vec& operator=(const vec&)=default;
  constexpr vec& operator=(vec&&)=default;

  constexpr auto operator<=>(const vec&) const = default;
  constexpr bool operator==(const vec&) const = default;
  constexpr bool operator!=(const vec&) const = default;
  constexpr bool operator<(const vec&) const = default;
  constexpr bool operator>(const vec&) const = default;
  constexpr bool operator<=(const vec&) const = default;
  constexpr bool operator>=(const vec&) const = default;

#define AOP(OP) \
  template<typename B> \
  constexpr vec& operator OP##=(const B& t){ \
    *this = *this OP t; \
    return *this; \
  }

  AOP(%);
  AOP(^);
  AOP(&);
  AOP(|);
  AOP(+);
  AOP(-);
  AOP(*);
  AOP(/);
#undef AOP

};

#define UOP(OP) \
template<typename T,size_t N> requires (N>4) \
constexpr vec<decltype(OP T()),N> operator OP (const vec<T,N>& a){ \
  vec<decltype(OP T()),N> ret; \
  for(int n=0;n<N;n++){ \
    ret[n] = OP a[n]; \
  } \
  return ret; \
}

UOP(~);
UOP(!);
UOP(-);
#undef UOP

#define BOP(OP) \
template<typename A, typename B, size_t N> requires (N>4) \
constexpr vec<decltype(A() OP B()),N> operator OP (const vec<A,N>& a, const vec<B,N>& b){ \
  vec<decltype(A() OP B()),N> ret; \
  for(int n=0;n<N;n++){ \
    ret[n] = a[n] OP b[n]; \
  } \
  return ret; \
}

BOP(%);
BOP(^);
BOP(&);
BOP(|);
BOP(&&);
BOP(||);
BOP(+);
BOP(-);
BOP(*);
BOP(/);
#undef BOP

typedef vec<float,2> fvec2;
typedef vec<float,3> fvec3;
typedef vec<float,4> fvec4;

typedef vec<double,2> dvec2;
typedef vec<double,3> dvec3;
typedef vec<double,4> dvec4;

typedef vec<int,2> ivec2;
typedef vec<int,3> ivec3;
typedef vec<int,4> ivec4;



template<typename T,size_t N>
std::string tostr(const vec<T,N>& v){
  std::string str="["+tostr(v.arr[0]);
  for(int n=1;n<N;n++){
    str+=", "+tostr(v.arr[n]);
  }
  return str+"]";
}

template<typename A,typename B>
constexpr auto dot(const vec<A,2>& a,const vec<B,2>& b){
  return a.x*b.x + a.y*b.y;
}

template<typename A,typename B>
constexpr auto dot(const vec<A,3>& a,const vec<B,3>& b){
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

template<typename A,typename B>
constexpr auto dot(const vec<A,4>& a,const vec<B,4>& b){
  return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

template<typename A, typename B,size_t N> requires (N>4)
constexpr auto dot(const vec<A,N>& a,const vec<B,N>& b){
  decltype(A()*B()) sum{};
  for(int n=0;n<N;n++){
    sum += a[n]*b[n];
  }
  return sum;
}

template<typename T,size_t N>
constexpr auto lensqr(const vec<T,N>& v){
  return dot(v,v);
}

template<typename T,size_t N>
constexpr auto len(const vec<T,N>& v){
  return sqrt(lensqr(v));
}

template<typename A,typename B>
constexpr vec<decltype(A()*B() - A()*B()),3> cross(const vec<A,3>& a, const vec<B,3>& b){
  return {a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]};
}

template<typename T,size_t N>
constexpr auto normalize(const vec<T,N>& v){
  return v/len(v);
}
