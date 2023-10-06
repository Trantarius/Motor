#pragma once
#include "Cycle.hpp"

class Updater;

class Updatable : public MemSafe{
protected:
  virtual void update(Updater*)=0;
public:
  friend class Updater;
};

struct Updater : public CYCLE(Updatable::update){};
