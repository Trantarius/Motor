#pragma once
#include <functional>
#include <type_traits>
#include "util/mem.hpp"
#include "util/collections.hpp"

template<typename T>
using Func = std::function<T>;

using Call = Func<void(void)>;

#define MemFunc(PTR,FN) std::bind(std::mem_fn(&std::remove_pointer<decltype(PTR)>::type::FN),PTR)

template<typename T>
class SafeFunc;

template<typename RET,typename...ARGS>
class SafeFunc<RET(ARGS...)>{
  Func<RET(ARGS...)> func;
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

typedef SafeFunc<void(void)> SafeCall;



template<typename F>
class Cycle;

template<typename RET,typename...ARGS>
class Cycle<RET(ARGS...)>{
public:
  typedef SafeFunc<RET(ARGS...)> Task;
private:
  List<SafeCall> pre_cycle;
  List<Task> cycle_list;
  List<SafeCall> post_cycle;
public:

  void add(Task task){
    cycle_list.push_back(task);
  }
  void remove(Task task){
    for(auto it=cycle_list.begin();it!=cycle_list.end();){
      if(!it->isCallable() || *it==task){
        it=cycle_list.erase(it);
      }else{
        it++;
      }
    }
  }

  void addPreCycle(SafeCall call){
    pre_cycle.push_back(call);
  }
  void removePreCycle(SafeCall call){
    for(auto it=pre_cycle.begin();it!=pre_cycle.end();){
      if(!it->isCallable() || *it==call){
        it=pre_cycle.erase(it);
      }else{
        it++;
      }
    }
  }

  void addPostCycle(SafeCall call){
    post_cycle.push_back(call);
  }
  void removePostCycle(SafeCall call){
    for(auto it=post_cycle.begin();it!=post_cycle.end();){
      if(!it->isCallable() || *it==call){
        it=post_cycle.erase(it);
      }else{
        it++;
      }
    }
  }

  void cycle(std::add_lvalue_reference<ARGS>::type...args){

    for(auto it=pre_cycle.begin();it!=pre_cycle.end();){
      if(!it->isCallable()){
        it=pre_cycle.erase(it);
      }else{
        it->call();
        it++;
      }
    }

    for(auto it=cycle_list.begin();it!=cycle_list.end();){
      if(!it->isCallable()){
        it=cycle_list.erase(it);
      }else{
        it->call(args...);
        it++;
      }
    }

    for(auto it=post_cycle.begin();it!=post_cycle.end();){
      if(!it->isCallable()){
        it=post_cycle.erase(it);
      }else{
        it->call();
        it++;
      }
    }
  }
};
