#pragma once
#include <cinttypes>
#include <cstdint>
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
  Bloc(size_t size):ptr(new T[size]{}),size(size){}
  Bloc(T* ptr, size_t size):ptr(ptr),size(size){}
  Bloc(Bloc& b):ptr(b.ptr),size(b.size){}
  template<typename...Ts> requires (std::is_same<Ts,T>::value && ...)
  Bloc(T a,Ts...args){
    ptr=new T[sizeof...(args)+1]{a,args...};
    size=sizeof...(args)+1;
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
    delete [] ptr;
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
    delete [] ptr;
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
