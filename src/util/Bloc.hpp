#pragma once
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <type_traits>


template<typename T=void>
struct Bloc{

	T* ptr=nullptr;
	size_t size=0;

	Bloc(){}
	Bloc(decltype(nullptr)){}
	Bloc(size_t size):size(size){
		ptr = (T*)(malloc(size*sizeof(T)));
		new(ptr) T[size]{};
	}
	Bloc(T* ptr, size_t size):ptr(ptr),size(size){}
	Bloc(Bloc& b):ptr(b.ptr),size(b.size){}
	template<typename...Ts> requires (std::is_same<Ts,T>::value && ...)
	Bloc(T a,Ts...args){
		size=sizeof...(args)+1;
		ptr = (T*)(malloc(size*sizeof(T)));
		new(ptr) T[size]{a,args...};
	}

	operator T*(){return ptr;}
	operator const T*() const {return ptr;}

	T& operator [] (size_t n){
		assert(n>=0&&n<size);
		return (ptr)[n];
	}

	const T& operator [] (size_t n) const {
		assert(n>=0&&n<size);
		return (ptr)[n];
	}

	bool is_null() const{
		return ptr==nullptr;
	}

	bool operator==(decltype(nullptr) null){
		return ptr==nullptr;
	}

	Bloc operator+(int i) const {
		return Bloc(ptr+i,size-i);
	}
	Bloc operator-(int i) const {
		return *this+-i;
	}

	void destroy(){
		assert(ptr!=nullptr);
		for(size_t i=0;i<size;i++){
			((T*)ptr+i)->~T();
		}
		free(ptr);
		ptr=nullptr;
		size=0;
	}
};

template <> struct Bloc<void>{
	void* ptr=nullptr;
	size_t size=0;

	Bloc(){}
	Bloc(decltype(nullptr)){}
	Bloc(void* ptr, size_t size):ptr(ptr),size(size){}
	Bloc(Bloc& b):ptr(b.ptr),size(b.size){}

	operator void*(){return ptr;}
	operator const void*() const {return ptr;}

	bool isNull() const{
		return ptr==nullptr;
	}

	bool operator==(decltype(nullptr) null){
		return ptr==nullptr;
	}

	void destroy(){
		assert(ptr!=nullptr);
		free(ptr);
		ptr=nullptr;
		size=0;
	}

	Bloc<void> operator+(int i) const {
		return Bloc<void>((u_char*)(ptr)+i,size-i);
	}
	Bloc<void> operator-(int i) const {
		return *this+-i;
	}
};
