#include "Render.hpp"

Render Render::main;

void Render::render(){
  assert(camera!=nullptr);
  current_projection=camera->getProjection();
  current_view=inverse(camera->getTransform());
  for(Call rcall : Render::main.render_calls){
    rcall();
  }
}

mat4 PerspectiveCamera::getProjection(){
  assert(renderer!=nullptr);
  return perspective(fov,(float)renderer->size.x/renderer->size.y,near,far);
}

mat4 OrthographicCamera::getProjection(){
  assert(renderer!=nullptr);
  float aspect=(float)renderer->size.x/renderer->size.y;
  return ortho(-size/aspect,size/aspect,-size,size,near,far);
}
