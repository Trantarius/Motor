#pragma once
#include "util/collections.hpp"
#include "util/funcy.hpp"
#include "main.hpp"
#include "util/mem.hpp"


template<typename F> class TimedCycle;

template<typename RET,typename...ARGS>
class TimedCycle<RET(ARGS...)> : public virtual Cycle<RET(ARGS...)>{
  List<double> start_times;
  double _cycle_time=0;
  double _dT=0;
  void preCycle(){
    double now=time();
    while(!start_times.empty() && now-start_times.front()>1){
      start_times.pop_front();
    }
    _dT=now-start_times.back();
    start_times.push_back(now);
  }

  void postCycle(){
    double now=time();
    _cycle_time=now-start_times.back();
  }
public:

  const double& cycle_time=_cycle_time;
  const double& dT=_dT;

  TimedCycle(){
    Cycle<RET(ARGS...)>::addPreCycle(SafeCall(this,&TimedCycle::preCycle));
    Cycle<RET(ARGS...)>::addPostCycle(SafeCall(this,&TimedCycle::postCycle));
  }

  double getRate() const {
    return start_times.size();
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
