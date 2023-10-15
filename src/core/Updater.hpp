#pragma once
#include "util/TimedCycle.hpp"

class Updater;

class Updatable : public virtual MemSafe{
protected:
  virtual void update(Updater*)=0;
public:
  friend class Updater;
};

struct Updater : public virtual STATIC_CYCLE(&Updatable::update), public virtual TimedCycle<void(Updater*)>{};
