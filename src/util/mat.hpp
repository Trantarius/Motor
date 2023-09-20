#pragma once
#include <cassert>
#include <cstring>
#include <array>
#include "vec.hpp"

//NB: this is row major
template<typename T,int N> requires (N>1)
struct mat : public vec<vec<T,N>,N>{

  mat(){}
  mat(std::initializer_list<vec<T,N>> li){
    assert(li.size()==N);
    memcpy(this->data,std::data(li),sizeof(T)*N*N);
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
mat<decltype(A()*B()),N> operator * (mat<A,N> a,mat<B,N> b){
  mat<decltype(A()*B()),N> ret;
  for(int n=0;n<N;n++){
    for(int m=0;m<N;m++){
      ret[n][m]=dot(a[n],b.col(m));
    }
  }
  return ret;
}

template<typename A,typename B,int N>
vec<decltype(A()*B()),N> operator * (mat<A,N> a,vec<B,N> b){
  vec<decltype(A()*B()),N> ret;
  for(int n=0;n<N;n++){
    ret[n]=dot(a[n],b);
  }
  return ret;
}

template<typename A,typename B,int N>
mat<decltype(A()+B()),N> operator + (mat<A,N> a,mat<B,N> b){
  mat<decltype(A()*B()),N> ret;
  for(int n=0;n<N;n++){
    for(int m=0;m<N;m++){
      ret[n][m]=a[n][m]+b[n][m];
    }
  }
  return ret;
}

template<typename A,typename B,int N>
mat<decltype(A()-B()),N> operator - (mat<A,N> a,mat<B,N> b){
  mat<decltype(A()*B()),N> ret;
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

template<typename T,int N>
mat<T,N> transpose(mat<T,N> mtx){
  mat<T,N> ret;
  for(int n=0;n<N;n++){
    for(int m=0;m<N;m++){
      ret[n][m]=mtx[m][n];
    }
  }
  return ret;
}
