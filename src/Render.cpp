#include "Render.hpp"
#include "Window.hpp"


void Render::preCycle(){
  assert(camera!=nullptr);
  current_projection=camera->getProjection(this);
  current_view=camera->getView(this);

  // update light buffer

  //Note: size must match max light count in light.glsl
  fvec3 colors[8];
  float lumins[8];
  float spreads[8];
  fmat4 transforms[8];
  int types[8];

  int idx=0;
  for(auto it=lights.begin();it!=lights.end();){
    if(!it->is_alive()){
      it=lights.erase(it);
      continue;
    }

    SafePtr<Light>& light=*it;
    colors[idx]=light->color;
    lumins[idx]=light->lumin;
    spreads[idx]=light->spread;
    transforms[idx]=current_view*light->transform.toMatrix();
    types[idx]=light->type;

    idx++;
    it++;
  }

  _light_buffer.beginUpdate();
  _light_buffer.setUniform("light_count"_id,idx);
  _light_buffer.setUniformArray("light_colors"_id,Bloc(colors,8));
  _light_buffer.setUniformArray("light_lumins"_id,Bloc(lumins,8));
  _light_buffer.setUniformArray("light_spreads"_id,Bloc(spreads,8));
  _light_buffer.setUniformArray("light_transforms"_id,Bloc(transforms,8));
  _light_buffer.setUniformArray("light_types"_id,Bloc(types,8));
  _light_buffer.endUpdate();

}

Render::Render(){
  addPreCycle(SafeCall(this,&Render::preCycle));
  Shader shader("/mesh.v.glsl","/lighttest.f.glsl");
  UniformBlock block=shader.getUniformBlock("LightBlock"_id);
  _light_buffer=UniformBuffer(block);
}

void Render::addLight(Light& light){
  SafePtr<Light> ptr(&light);
  lights.push_back(ptr);
}

void Render::removeLight(Light& light){
  SafePtr<Light> ptr(&light);
  for(auto it=lights.begin();it!=lights.end();){
    if(*it==ptr){
      it=lights.erase(it);
    }else{
      it++;
    }
  }
}

fmat4 Camera::getView(Render* renderer) const {
  return transform.toInvMatrix();
}

fmat4 PerspectiveCamera::getProjection(Render* renderer) const {
  //http://www.songho.ca/opengl/gl_projectionmatrix.html
  assert(renderer!=nullptr);
  float height=tan(fov/2)*near;
  float width=height * (float)renderer->getSize().x/renderer->getSize().y;
  return fmat4(
    near/width,   0,    0,    0,
    0,    near/height,    0,    0,
    0,    0,    (far+near)/(near-far), 2*far*near/(near-far),
    0,    0,    -1,   0
  );
}

fmat4 OrthographicCamera::getProjection(Render* renderer) const {
  assert(renderer!=nullptr);
  float aspect=(float)renderer->getSize().x/renderer->getSize().y;
  float height=size;
  float width=height*aspect;
  return fmat4(
    1/width,    0,    0,    0,
    0,    1/height,   0,    0,
    0,    0,    2/(near-far),   (near+far)/(near-far),
    0,    0,    0,    1
  );
}

void WindowRender::preRender(){
  glfwMakeContextCurrent(window.glfw());
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void WindowRender::postRender(){
  glfwSwapBuffers(window.glfw());
}

void WindowRender::setSize(ivec2 to){
  window.setSize(to);
}
ivec2 WindowRender::getSize() const {
  return window.getSize();
}

WindowRender::WindowRender(Window& win):window(win){
  addPreCycle(SafeFunc<void()>(this,&WindowRender::preRender));
  addPostCycle(SafeFunc<void()>(this,&WindowRender::postRender));
}
