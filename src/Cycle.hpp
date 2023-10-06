#pragma once
#include "util/collections.hpp"
#include "util/mem.hpp"
#include "main.hpp"


template<typename T>
class _memftype;

template<typename R,typename C,typename A>
struct _memftype<R (C::*)(A)>{
  typedef R RETURN;
  typedef C CLASS;
  typedef A ARGUMENT;
};


template<typename F,F FUNC> requires std::is_base_of<MemSafe,typename _memftype<F>::CLASS>::value
class Cycle;

template<typename T,typename C,void(T::* FUNC)(C)> requires std::is_base_of<MemSafe,T>::value
class Cycle<decltype(FUNC),FUNC>{
  List<SafePtr<T>> cycle_list;
  double last_time=time();
public:

  double deltaT;

  void add(T* ptr){cycle_list.push_back(SafePtr<T>(ptr));}
  void remove(T* ptr){
    for(auto it=cycle_list.begin();it!=cycle_list.end();){
      if(!it->is_alive() || it->get_ptr()==ptr){
        it=cycle_list.erase(it);
      }else{
        it++;
      }
    }
  }

  virtual void pre_cycle(){}
  virtual void post_cycle(){}
  void cycle(){

    double now=time();
    deltaT=now-last_time;
    last_time=now;

    pre_cycle();

    for(auto it=cycle_list.begin();it!=cycle_list.end();){
      if(!it->is_alive()){
        it=cycle_list.erase(it);
      }else{
        ((*it).get_ptr()->*FUNC)((C)this);
        it++;
      }
    }

    post_cycle();
  }
};


#define CYCLE(memfunc) Cycle<decltype(&memfunc),&memfunc>
