#pragma once
#include <cassert>
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

template<typename T,int N>
string tostr(const mat<T,N>& mtx){
  string ret;
  for(int n=0;n<N;n++){
    ret+=tostr(mtx[n])+"\n";
  }
  return ret;
}


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

template<NumberType T>
mat<T,2> inverse(const mat<T,2>& mtx){
  T det=1.0/(mtx[0][0]*mtx[1][1]-mtx[0][1]*mtx[1][0]);
  return mat<T,2>(
    mtx[1][1]/det,    -mtx[0][1]/det,
    -mtx[1][0]/det,   mtx[0][0]/det
  );
}

template<NumberType T>
mat<T,3> inverse(const mat<T,3>& mtx){

  mat<T,3> ret;
  T* out=(T*)&ret;
  const T* M=(const T*)&mtx;

  const T x0 = 1.0/M[0];
  const T x1 = x0*M[6];
  const T x2 = -x1*M[1] + M[7];
  const T x3 = x0*M[3];
  const T x4 = 1.0/(-x3*M[1] + M[4]);
  const T x5 = x0*x2*x4*M[3] - x1;
  const T x6 = -x3*M[2] + M[5];
  const T x7 = x4*x6;
  const T x8 = x2*x7;
  const T x9 = 1.0/(-x1*M[2] - x8 + M[8]);
  const T x10 = x9*M[2];
  const T x11 = x5*x9;
  const T x12 = x4*(-x11*x6 - x3);
  const T x13 = x4*(x8*x9 + 1);

  out[0] = x0*(-x10*x5 - x12*M[1] + 1);
  out[1] = x0*(-x13*M[1] + x2*x4*x9*M[2]);
  out[2] = x0*(-x10 + x4*x6*x9*M[1]);
  out[3] = x12;
  out[4] = x13;
  out[5] = -x7*x9;
  out[6] = x11;
  out[7] = -x2*x4*x9;
  out[8] = x9;

  return ret;
}

template<NumberType T>
mat<T,4> inverse(const mat<T,4>& mtx){

  mat<T,4> ret;
  T* out=(T*)&ret;
  const T* M=(const T*)&mtx;

  const T x0 = 1.0/M[0];
  const T x1 = x0*M[12];
  const T x2 = -x1*M[1] + M[13];
  const T x3 = x0*M[4];
  const T x4 = 1.0/(-x3*M[1] + M[5]);
  const T x5 = x0*M[8];
  const T x6 = -x5*M[1] + M[9];
  const T x7 = -x0*x4*x6*M[4] + x5;
  const T x8 = -x3*M[2] + M[6];
  const T x9 = x4*x6;
  const T x10 = 1.0/(-x5*M[2] - x8*x9 + M[10]);
  const T x11 = x2*x4;
  const T x12 = -x1*M[2] - x11*x8 + M[14];
  const T x13 = x10*x12;
  const T x14 = x0*x2*x4*M[4] - x1 + x13*x7;
  const T x15 = -x3*M[3] + M[7];
  const T x16 = -x15*x9 - x5*M[3] + M[11];
  const T x17 = x13*x16;
  const T x18 = 1.0/(-x1*M[3] - x11*x15 - x17 + M[15]);
  const T x19 = x18*M[3];
  const T x20 = x14*x18;
  const T x21 = x10*(-x16*x20 - x7);
  const T x22 = x15*x18;
  const T x23 = x4*(-x14*x22 - x21*x8 - x3);
  const T x24 = -x11 + x13*x9;
  const T x25 = x18*x24;
  const T x26 = x10*(-x16*x25 - x9);
  const T x27 = x4*(-x22*x24 - x26*x8 + 1);
  const T x28 = x10*(x17*x18 + 1);
  const T x29 = x4*(x10*x12*x15*x18 - x28*x8);
  const T x30 = x10*x16*x18;
  const T x31 = x4*(-x22 + x30*x8);

  out[0] = x0*(-x14*x19 - x21*M[2] - x23*M[1] + 1);
  out[1] = x0*(-x19*x24 - x26*M[2] - x27*M[1]);
  out[2] = x0*(x10*x12*x18*M[3] - x28*M[2] - x29*M[1]);
  out[3] = x0*(x10*x16*x18*M[2] - x19 - x31*M[1]);
  out[4] = x23;
  out[5] = x27;
  out[6] = x29;
  out[7] = x31;
  out[8] = x21;
  out[9] = x26;
  out[10] = x28;
  out[11] = -x30;
  out[12] = x20;
  out[13] = x25;
  out[14] = -x13*x18;
  out[15] = x18;

  return ret;
}
