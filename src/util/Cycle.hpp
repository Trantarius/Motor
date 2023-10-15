#pragma once
#include "collections.hpp"
#include "meta.hpp"
#include "MemSafe.hpp"



template<typename F>
class Cycle;

template<typename RET,typename...ARGS>
class Cycle<RET(ARGS...)>{
public:
  typedef SafeFunc<RET(ARGS...)> Task;
private:
  List<SafeFunc<void()>> pre_cycle;
  List<Task> cycle_list;
  List<SafeFunc<void()>> post_cycle;
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

  void addPreCycle(SafeFunc<void()> call){
    pre_cycle.push_back(call);
  }
  void removePreCycle(SafeFunc<void()> call){
    for(auto it=pre_cycle.begin();it!=pre_cycle.end();){
      if(!it->isCallable() || *it==call){
        it=pre_cycle.erase(it);
      }else{
        it++;
      }
    }
  }

  void addPostCycle(SafeFunc<void()> call){
    post_cycle.push_back(call);
  }
  void removePostCycle(SafeFunc<void()> call){
    for(auto it=post_cycle.begin();it!=post_cycle.end();){
      if(!it->isCallable() || *it==call){
        it=post_cycle.erase(it);
      }else{
        it++;
      }
    }
  }

  void cycle(AutoPassType<ARGS>::type...args){

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



template<typename F,F FUNC> class StaticCycle;

template<typename CLASS,typename RET,typename...ARGS,RET (CLASS::* FUNC)(ARGS...)>
struct StaticCycle<RET(CLASS::*)(ARGS...),FUNC> : virtual Cycle<RET(ARGS...)>{

  void add(CLASS& obj){
    Cycle<RET(ARGS...)>::add(SafeFunc<RET(ARGS...)>(&obj,FUNC));
  }
  void remove(CLASS& obj){
    Cycle<RET(ARGS...)>::remove(SafeFunc<RET(ARGS...)>(&obj,FUNC));
  }
};


#define STATIC_CYCLE(FN) StaticCycle<decltype(FN),FN>
