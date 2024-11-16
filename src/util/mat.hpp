#pragma once
#include <cassert>
#include "vec.hpp"

template<typename T, size_t N, size_t M>
struct mat{
	T arr[N*M];

	T& operator[](int n){
		return arr[n];
	}
	const T& operator[](int n) const {
		return arr[n];
	}

	T& operator[](ivec2 nm){
		return arr[nm.x*M+nm.y];
	}
	const T& operator[](ivec2 nm) const {
		return arr[nm.x*M+nm.y];
	}

	constexpr mat()=default;
	constexpr mat(const mat&)=default;
	constexpr mat(mat&&)=default;
	constexpr mat& operator=(const mat&)=default;
	constexpr mat& operator=(mat&&)=default;

	constexpr std::strong_ordering operator<=>(const mat&) const = default;
	constexpr bool operator==(const mat&) const = default;
	constexpr bool operator!=(const mat&) const = default;
	constexpr bool operator<(const mat&) const = default;
	constexpr bool operator>(const mat&) const = default;
	constexpr bool operator<=(const mat&) const = default;
	constexpr bool operator>=(const mat&) const = default;

	template<typename...Ts> requires (sizeof...(Ts)==N*M)
	constexpr mat(Ts...args): arr{(T)args...}{}

	template<typename B>
	constexpr mat(const mat<B,N,M>& b){
		for(int n=0;n<N;n++){
			for(int m=0;m<M;m++){
				(*this)[{n,m}] = b[{n,m}];
			}
		}
	}

	constexpr static mat identity(){
		mat mt;
		for(int n=0;n<N;n++){
			for(int m=0;m<M;m++){
				mt[{n,m}] = (n==m?1:0);
			}
		}
		return mt;
	}

	constexpr vec<T,M> get_row(int n) const {
		vec<T,M> ret;
		for(int m=0;m<M;m++){
			ret[m] = (*this)[{n,m}];
		}
		return ret;
	}
	constexpr vec<T,N> get_col(int m) const {
		vec<T,N> ret;
		for(int n=0;n<N;n++){
			ret[n] = (*this)[{n,m}];
		}
		return ret;
	}
	constexpr void set_row(int n, const vec<T,M>& r){
		for(int m=0;m<M;m++){
			(*this)[{n,m}] = r[m];
		}
	}
	constexpr void set_col(int m, const vec<T,N>& r){
		for(int n=0;n<N;n++){
			(*this)[{n,m}] = r[n];
		}
	}

#define AOP(OP) \
	template<typename B> \
	constexpr mat& operator OP##=(const B& t){ \
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










typedef mat<float,2,2> fmat2;
typedef mat<float,2,3> fmat2x3;
typedef mat<float,2,4> fmat2x4;
typedef mat<float,3,2> fmat3x2;
typedef mat<float,3,3> fmat3;
typedef mat<float,3,4> fmat3x4;
typedef mat<float,4,2> fmat4x2;
typedef mat<float,4,3> fmat4x3;
typedef mat<float,4,4> fmat4;

typedef mat<double,2,2> dmat2;
typedef mat<double,2,3> dmat2x3;
typedef mat<double,2,4> dmat2x4;
typedef mat<double,3,2> dmat3x2;
typedef mat<double,3,3> dmat3;
typedef mat<double,3,4> dmat3x4;
typedef mat<double,4,2> dmat4x2;
typedef mat<double,4,3> dmat4x3;
typedef mat<double,4,4> dmat4;

template<typename A, typename B, size_t N, size_t M, size_t O>
mat<decltype(A()*B()+A()*B()),N,O> operator * (const mat<A,N,M>& a, const mat<B,M,O>& b){
	mat<decltype(A()*B()+A()*B()),N,O> ret;
	for(int n=0;n<N;n++){
		for(int o=0;o<O;o++){
			ret[{n,o}] = dot(a.get_row(n),b.get_col(o));
		}
	}
	return ret;
}

template<typename A, typename B, size_t N, size_t M>
vec<decltype(A()*B()+A()*B()),N> operator * (const mat<A,N,M>& a, const vec<B,M>& b){
	vec<decltype(A()*B()+A()*B()),N> ret;
	for(int n=0;n<N;n++){
		ret[n] = dot(a.get_row(n),b);
	}
}

template<typename A, typename B, size_t N, size_t M>
vec<decltype(A()*B()+A()*B()),M> operator * (const vec<A,N>& a, const mat<B,N,M>& b){
	vec<decltype(A()*B()+A()*B()),M> ret;
	for(int m=0;m<M;m++){
		ret[m] = dot(a,b.get_col(m));
	}
}

template<typename A, typename B, size_t N, size_t M>
mat<decltype(A()+B()),N,M> operator + (const mat<A,N,M>& a, const mat<B,N,M>& b){
	mat<decltype(A()+B()),N,M> ret;
	for(int n=0;n<N;n++){
		for(int m=0;m<M;m++){
			ret[{n,m}] = a[{n,m}] + b[{n,m}];
		}
	}
	return ret;
}

template<typename A, typename B, size_t N, size_t M>
mat<decltype(A()-B()),N,M> operator - (const mat<A,N,M>& a, const mat<B,N,M>& b){
	mat<decltype(A()-B()),N,M> ret;
	for(int n=0;n<N;n++){
		for(int m=0;m<M;m++){
			ret[{n,m}] = a[{n,m}] - b[{n,m}];
		}
	}
	return ret;
}

template<typename A, size_t N, size_t M>
mat<decltype(-A()),N,M> operator - (const mat<A,N,M>& a){
	mat<decltype(-A()),N,M> ret;
	for(int n=0;n<N;n++){
		for(int m=0;m<M;m++){
			ret[{n,m}] = - a[{n,m}];
		}
	}
	return ret;
}



template<typename T,size_t N, size_t M>
std::string tostr(const mat<T,N,M>& mtx){
	std::string ret;
	for(int n=0;n<N;n++){
		ret+=tostr(mtx.get_row(n))+"\n";
	}
	return ret;
}


template<typename T,size_t N, size_t M>
mat<T,M,N> transpose(const mat<T,N,M>& mtx){
	mat<T,M,N> ret;
	for(int n=0;n<N;n++){
		for(int m=0;m<M;m++){
			ret[{m,n}]=mtx[{n,m}];
		}
	}
	return ret;
}

template<typename T>
mat<T,2,2> inverse(const mat<T,2,2>& mtx){
	T det=1.0/(mtx[{0,0}]*mtx[{1,1}]-mtx[{0,1}]*mtx[{1,0}]);
	return mat<T,2,2>(
		mtx[{1,1}]/det,    -mtx[{0,1}]/det,
		-mtx[{1,0}]/det,   mtx[{0,0}]/det
	);
}

template<typename T>
mat<T,3,3> inverse(const mat<T,3,3>& mtx){

	mat<T,3,3> ret;

	const T x0 = 1.0/mtx[0];
	const T x1 = x0*mtx[6];
	const T x2 = -x1*mtx[1] + mtx[7];
	const T x3 = x0*mtx[3];
	const T x4 = 1.0/(-x3*mtx[1] + mtx[4]);
	const T x5 = x0*x2*x4*mtx[3] - x1;
	const T x6 = -x3*mtx[2] + mtx[5];
	const T x7 = x4*x6;
	const T x8 = x2*x7;
	const T x9 = 1.0/(-x1*mtx[2] - x8 + mtx[8]);
	const T x10 = x9*mtx[2];
	const T x11 = x5*x9;
	const T x12 = x4*(-x11*x6 - x3);
	const T x13 = x4*(x8*x9 + 1);

	ret[0] = x0*(-x10*x5 - x12*mtx[1] + 1);
	ret[1] = x0*(-x13*mtx[1] + x2*x4*x9*mtx[2]);
	ret[2] = x0*(-x10 + x4*x6*x9*mtx[1]);
	ret[3] = x12;
	ret[4] = x13;
	ret[5] = -x7*x9;
	ret[6] = x11;
	ret[7] = -x2*x4*x9;
	ret[8] = x9;

	return ret;
}

template<typename T>
mat<T,4,4> inverse(const mat<T,4,4>& mtx){

	mat<T,4,4> ret;

	const T x0 = 1.0/mtx[0];
	const T x1 = x0*mtx[12];
	const T x2 = -x1*mtx[1] + mtx[13];
	const T x3 = x0*mtx[4];
	const T x4 = 1.0/(-x3*mtx[1] + mtx[5]);
	const T x5 = x0*mtx[8];
	const T x6 = -x5*mtx[1] + mtx[9];
	const T x7 = -x0*x4*x6*mtx[4] + x5;
	const T x8 = -x3*mtx[2] + mtx[6];
	const T x9 = x4*x6;
	const T x10 = 1.0/(-x5*mtx[2] - x8*x9 + mtx[10]);
	const T x11 = x2*x4;
	const T x12 = -x1*mtx[2] - x11*x8 + mtx[14];
	const T x13 = x10*x12;
	const T x14 = x0*x2*x4*mtx[4] - x1 + x13*x7;
	const T x15 = -x3*mtx[3] + mtx[7];
	const T x16 = -x15*x9 - x5*mtx[3] + mtx[11];
	const T x17 = x13*x16;
	const T x18 = 1.0/(-x1*mtx[3] - x11*x15 - x17 + mtx[15]);
	const T x19 = x18*mtx[3];
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

	ret[0] = x0*(-x14*x19 - x21*mtx[2] - x23*mtx[1] + 1);
	ret[1] = x0*(-x19*x24 - x26*mtx[2] - x27*mtx[1]);
	ret[2] = x0*(x10*x12*x18*mtx[3] - x28*mtx[2] - x29*mtx[1]);
	ret[3] = x0*(x10*x16*x18*mtx[2] - x19 - x31*mtx[1]);
	ret[4] = x23;
	ret[5] = x27;
	ret[6] = x29;
	ret[7] = x31;
	ret[8] = x21;
	ret[9] = x26;
	ret[10] = x28;
	ret[11] = -x30;
	ret[12] = x20;
	ret[13] = x25;
	ret[14] = -x13*x18;
	ret[15] = x18;

	return ret;
}
