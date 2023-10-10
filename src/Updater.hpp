#pragma once
#include "Cycle.hpp"

class Updater;

class Updatable : public virtual MemSafe{
protected:
  virtual void update(Updater*)=0;
public:
  friend class Updater;
};

struct Updater : public STATIC_CYCLE(&Updatable::update), public virtual TimedCycle<void(Updater*)>{};
