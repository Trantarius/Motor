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

template<typename T>
mat<T,2> inverse(const mat<T,2>& mtx){
  T det=1.0/(mtx[0][0]*mtx[1][1]-mtx[0][1]*mtx[1][0]);
  return mat<T,2>(
    mtx[1][1]/det,    -mtx[0][1]/det,
    -mtx[1][0]/det,   mtx[0][0]/det
  );
}

template<typename T>
mat<T,3> inverse(const mat<T,3>& mtx){
#define A mtx[0][0]
#define B mtx[0][1]
#define C mtx[0][2]
#define D mtx[1][0]
#define E mtx[1][1]
#define F mtx[1][2]
#define G mtx[2][0]
#define H mtx[2][1]
#define I mtx[2][2]

  T det=1.0/(A*E*I - A*F*H - B*D*I + B*F*G + C*D*H + C*E*G);
  return mat<T,3>(
    (E*I-F*H)/det,  (C*H-B*I)/det,  (B*F-C*E)/det,
    (F*G-D*I)/det,  (A*I-C*G)/det,  (C*D-A*F)/det,
    (D*H-E*G)/det,  (B*G-A*H)/det,  (A*E-B*D)/det
  );

#undef A
#undef B
#undef C
#undef D
#undef E
#undef F
#undef G
#undef H
#undef I
}

template<typename T>
mat<T,4> inverse(const mat<T,4>& mtx){

#define A mtx[0][0]
#define B mtx[0][1]
#define C mtx[0][2]
#define D mtx[0][3]
#define E mtx[1][0]
#define F mtx[1][1]
#define G mtx[1][2]
#define H mtx[1][3]
#define I mtx[2][0]
#define J mtx[2][1]
#define K mtx[2][2]
#define L mtx[2][3]
#define M mtx[3][0]
#define N mtx[3][1]
#define O mtx[3][2]
#define P mtx[3][3]

T det = D*G*J*M - C*H*J*M - D*F*K*M + B*H*K*M + C*F*L*M - B*G*L*M -
        D*G*I*N + C*H*I*N + D*E*K*N - A*H*K*N - C*E*L*N + A*G*L*N +
        D*F*I*O - B*H*I*O - D*E*J*O + A*H*J*O + B*E*L*O - A*F*L*O -
        C*F*I*P + B*G*I*P + C*E*J*P - A*G*J*P - B*E*K*P + A*F*K*P;

  return mat<T,4>(
    (-H*K*N + G*L*N + H*J*O - F*L*O - G*J*P + F*K*P)/det,
    (D*K*N - C*L*N - D*J*O + B*L*O + C*J*P - B*K*P)/det,
    (-D*G*N + C*H*N + D*F*O - B*H*O - C*F*P + B*G*P)/det,
    (D*G*J - C*H*J - D*F*K + B*H*K + C*F*L - B*G*L)/det,

    (H*K*M - G*L*M - H*I*O + E*L*O + G*I*P - E*K*P)/det,
    (-D*K*M + C*L*M + D*I*O - A*L*O - C*I*P + A*K*P)/det,
    (D*G*M - C*H*M - D*E*O + A*H*O + C*E*P - A*G*P)/det,
    (-D*G*I + C*H*I + D*E*K - A*H*K - C*E*L + A*G*L)/det,

    (-H*J*M + F*L*M + H*I*N - E*L*N - F*I*P + E*J*P)/det,
    (D*J*M - B*L*M - D*I*N + A*L*N + B*I*P - A*J*P)/det,
    (-D*F*M + B*H*M + D*E*N - A*H*N - B*E*P + A*F*P)/det,
    (D*F*I - B*H*I - D*E*J + A*H*J + B*E*L - A*F*L)/det,

    (G*J*M - F*K*M - G*I*N + E*K*N + F*I*O - E*J*O)/det,
    (-C*J*M + B*K*M + C*I*N - A*K*N - B*I*O + A*J*O)/det,
    (C*F*M - B*G*M - C*E*N + A*G*N + B*E*O - A*F*O)/det,
    (-C*F*I + B*G*I + C*E*J - A*G*J - B*E*K + A*F*K)/det
  );

#undef A
#undef B
#undef C
#undef D
#undef E
#undef F
#undef G
#undef H
#undef I
#undef J
#undef K
#undef L
#undef M
#undef N
#undef O
#undef P
}
