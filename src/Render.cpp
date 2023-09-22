#include "Render.hpp"

Render Render::main;

void Render::render(){
  for(Call rcall : Render::main.render_calls){
    rcall();
  }
}
