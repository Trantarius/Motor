#pragma once
#include "util/collections.hpp"

class Renderable{
  List<Renderable*>::iterator renderables_pos;
protected:
  virtual void render()=0;
public:
  Renderable();
  ~Renderable();
  friend void renderAll();
};

void renderAll();
