#pragma once
#include <cinttypes>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cassert>


template<typename T>
struct Bloc{

  T* ptr=nullptr;
  size_t size=0;

  Bloc(){}
  Bloc(size_t size):ptr(new T[size]{}),size(size){}
  Bloc(T* ptr, size_t size):ptr(ptr),size(size){}
  Bloc(Bloc& b):ptr(b.ptr),size(b.size){}

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

  void destroy(){
    assert(ptr!=nullptr);
    delete [] ptr;
    ptr=nullptr;
    size=0;
  }
};
