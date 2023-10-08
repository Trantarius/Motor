#pragma once
#include <cassert>
#include <cstring>
#include <array>
#include "vec.hpp"

//NB: this is row major
template<typename T,int N> requires (N>1)
struct mat : public vec<vec<T,N>,N>{

  mat(){}
  template<typename...Ts> requires (std::is_convertible<Ts,T>::value&&...) && (sizeof...(Ts)==N*N)
  mat(Ts...args){
    T arr[N*N]{((T)args)...};
    memcpy(this->data,arr,sizeof(T)*N*N);
  }

  template<typename _T,int _N> requires std::is_convertible<_T,T>::value
  mat(const mat<_T,_N>& b){
    for(int n=0;n<std::min(_N,N);n++){
      for(int m=0;m<std::min(_N,N);m++){
        (*this)[n][m]=(T)b[n][m];
      }
    }
    for(int n=std::min(_N,N);n<std::max(_N,N);n++){
      (*this)[n][n]=1;
    }
  }

  static mat identity(){
    mat m;
    for(int n=0;n<N;n++){
      m[n][n]=1;
    }
    return m;
  }

  vec<T,N> col(int n) const {
    vec<T,N> ret;
    for(int r=0;r<N;r++){
      ret[r]=this->data[r][n];
    }
    return ret;
  }
};

typedef mat<float,2> fmat2;
typedef mat<float,3> fmat3;
typedef mat<float,4> fmat4;

typedef mat<double,2> dmat2;
typedef mat<double,3> dmat3;
typedef mat<double,4> dmat4;

template<typename A,typename B,int N>
mat<decltype(A()*B()),N> operator * (const mat<A,N>& a,const mat<B,N>& b){
  mat<decltype(A()*B()),N> ret;
  for(int n=0;n<N;n++){
    for(int m=0;m<N;m++){
      ret[n][m]=dot(a[n],b.col(m));
    }
  }
  return ret;
}

template<typename A,typename B,int N>
vec<decltype(A()*B()),N> operator * (const mat<A,N>& a,const vec<B,N>& b){
  vec<decltype(A()*B()),N> ret;
  for(int n=0;n<N;n++){
    ret[n]=dot(a[n],b);
  }
  return ret;
}

template<typename A,typename B,int N>
mat<decltype(A()+B()),N> operator + (const mat<A,N>& a,const mat<B,N>& b){
  mat<decltype(A()+B()),N> ret;
  for(int n=0;n<N;n++){
    for(int m=0;m<N;m++){
      ret[n][m]=a[n][m]+b[n][m];
    }
  }
  return ret;
}

template<typename A,typename B,int N>
mat<decltype(A()-B()),N> operator - (const mat<A,N>& a,const mat<B,N>& b){
  mat<decltype(A()-B()),N> ret;
  for(int n=0;n<N;n++){
    for(int m=0;m<N;m++){
      ret[n][m]=a[n][m]-b[n][m];
    }
  }
  return ret;
}

#define MAT_EQ_OP(OP)\
template<typename A,typename B,int N>\
void operator OP##= (mat<A,N>& a,mat<B,N> b){\
  a = a OP b;\
}

MAT_EQ_OP(*);
MAT_EQ_OP(-);
MAT_EQ_OP(+);

#undef MAT_EQ_OP

template<Printable T,int N>
string tostr(const mat<T,N>& mtx){
  string ret;
  for(int n=0;n<N;n++){
    ret+=tostr(mtx[n])+"\n";
  }
  return ret;
}
