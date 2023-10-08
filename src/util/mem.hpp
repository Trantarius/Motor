#pragma once
#include <memory>
#include <assert.h>

template<typename T>
using Unique=std::unique_ptr<T>;

template<typename T>
using Shared=std::shared_ptr<T>;

template<typename T>
using Weak=std::weak_ptr<T>;


class MemSafe{
  Shared<bool> alive;
public:
  MemSafe():alive(new bool(true)){}
  MemSafe(const MemSafe& b):alive(new bool(true)){}
  void operator=(const MemSafe& b){}

  ~MemSafe(){
    *alive=false;
  }

  template<typename T> requires std::is_base_of<MemSafe,T>::value
  friend class SafePtr;
  template<typename T>
  friend class SafeFunc;
};

template<typename T> requires std::is_base_of<MemSafe,T>::value
class SafePtr{
  T* ptr=nullptr;
  Shared<bool> alive=nullptr;
public:
  T& operator * (){
    assert(alive);
    assert(*alive);
    return *ptr;
  }
  const T& operator * () const {
    assert(alive);
    assert(*alive);
    return *ptr;
  }

  T* operator -> (){
    assert(alive);
    assert(*alive);
    return ptr;
  }
  const T* operator -> () const {
    assert(alive);
    assert(*alive);
    return ptr;
  }

  T* get_ptr(){
    assert(alive);
    assert(*alive);
    return ptr;
  }
  const T* get_ptr() const {
    assert(alive);
    assert(*alive);
    return ptr;
  }

  bool is_null() const {
    return ptr==nullptr;
  }
  bool is_alive() const {
    assert(alive);
    return *alive;
  }

  SafePtr(){}
  SafePtr(T* ptr):ptr(ptr),alive(ptr==nullptr?nullptr:ptr->alive){}

  void operator=(T* _ptr){
    ptr=_ptr;
    if(ptr==nullptr){
      alive=nullptr;
    }else{
      alive=ptr->alive;
    }
  }
};
