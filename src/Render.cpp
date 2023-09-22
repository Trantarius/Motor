#include "Render.hpp"

List<Renderable*> renderables;

Renderable::Renderable(){
  renderables.push_back(this);
  renderables_pos=--renderables.end();
}

Renderable::~Renderable(){
  renderables.erase(renderables_pos);
}

void renderAll(){
  for(Renderable* rptr : renderables){
    rptr->render();
  }
}
