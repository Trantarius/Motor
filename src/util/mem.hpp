#pragma once
#include <memory>
#include <assert.h>
#include "strings.hpp"

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

struct _ref_count_init{};

#ifndef NDEBUG
//dummy ref prevents a constructor from not initializing the ref counted data holder
constexpr _ref_count_init _global_ref_count_dummy;
#define _REF_COUNT_DUMMY_REF const _ref_count_init& _ref_count_dummy_ref;
#define _REF_COUNT_DUMMY_REF_INIT :_ref_count_dummy_ref(_global_ref_count_dummy)
#else
#define _REF_COUNT_DUMMY_REF
#define _REF_COUNT_DUMMY_REF_INIT
#endif

#define REF_COUNTER(CLASS,DATA)                              \
private:                                                     \
  DATA* data=nullptr;                                        \
  _REF_COUNT_DUMMY_REF                                       \
  void onDestroy();                                          \
                                                             \
  void dispose(){                                            \
    onDestroy();                                             \
    delete data;                                             \
    data=nullptr;                                            \
  }                                                          \
                                                             \
  CLASS(_ref_count_init)_REF_COUNT_DUMMY_REF_INIT{           \
    data=new DATA();                                         \
    data->refcount+=1;                                       \
  }                                                          \
                                                             \
public:                                                      \
                                                             \
  CLASS()_REF_COUNT_DUMMY_REF_INIT{}                         \
                                                             \
                                                             \
  CLASS(const CLASS& b)_REF_COUNT_DUMMY_REF_INIT{            \
    *this=b;                                                 \
  }                                                          \
                                                             \
  void operator = (const CLASS& b){                          \
    if(data!=nullptr){                                       \
      data->refcount--;                                      \
      if(data->refcount==0){                                 \
        dispose();                                           \
      }                                                      \
    }                                                        \
                                                             \
    data=b.data;                                             \
    if(data!=nullptr){                                       \
      data->refcount++;                                      \
    }                                                        \
  }                                                          \
                                                             \
  bool isNull() const{                                       \
    return data==nullptr;                                    \
  }                                                          \
                                                             \
  ~CLASS(){                                                  \
    if(data!=nullptr){                                       \
      data->refcount--;                                      \
      if(data->refcount==0){                                 \
        dispose();                                           \
      }                                                      \
    }                                                        \
  }
