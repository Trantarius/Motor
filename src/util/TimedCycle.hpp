#pragma once
#include "Cycle.hpp"
#include "time.hpp"

template<typename F> class TimedCycle;

template<typename RET,typename...ARGS>
class TimedCycle<RET(ARGS...)> : public virtual Cycle<RET(ARGS...)>, public virtual MemSafe{
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
    Cycle<RET(ARGS...)>::addPreCycle(SafeFunc<void()>(this,&TimedCycle::preCycle));
    Cycle<RET(ARGS...)>::addPostCycle(SafeFunc<void()>(this,&TimedCycle::postCycle));
  }

  double getRate() const {
    return start_times.size();
  }
};
