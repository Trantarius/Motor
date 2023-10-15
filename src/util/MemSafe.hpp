#pragma once
#include <memory>
#include <assert.h>
#include <functional>

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

  bool operator==(const SafePtr& b){
    return ptr==b.ptr;
  }
};



template<typename T>
class SafeFunc;

template<typename RET,typename...ARGS>
class SafeFunc<RET(ARGS...)>{
  std::function<RET(ARGS...)> func;
  bool is_mem_func=false;
  Shared<bool> alive=nullptr;
  uint64_t raw_fptr[2];//used for comparison

  SafeFunc(){}
public:

  RET call(ARGS...args) const {
    assert(!is_mem_func || alive && *alive);
    assert(func);
    return func(args...);
  }

  bool isNull() const {
    return alive==nullptr || !func;
  }

  bool isAlive() const {
    if(is_mem_func){
      assert(alive);
      return *alive;
    }
    else{
      return true;
    }
  }

  bool isCallable() const {
    if(is_mem_func){
      return alive && *alive && func;
    }else{
      return (bool)func;
    }
  }

  bool isMemFunc() const {
    return is_mem_func;
  }

  SafeFunc(RET (*fptr)(ARGS...)):func(fptr){
    raw_fptr[0]=fptr;
    raw_fptr[1]=fptr;
  }

  template<typename T> requires std::is_base_of<MemSafe,T>::value
  SafeFunc(T* optr,RET(T::*fptr)(ARGS...)): is_mem_func(true), alive(optr->alive){

    func = [fptr,optr](ARGS...args){(optr->*fptr)(args...);};

    static_assert(sizeof(fptr)<=16);
    raw_fptr[0]=0;
    raw_fptr[1]=0;
    memcpy(raw_fptr,&fptr,sizeof(fptr));
  }

  bool operator==(const SafeFunc& b){
    return is_mem_func==b.is_mem_func && raw_fptr[0]==b.raw_fptr[0] && raw_fptr[1]==b.raw_fptr[1] && (!is_mem_func || alive==b.alive);
  }

};

