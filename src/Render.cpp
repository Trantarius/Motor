#include "Render.hpp"
#include "Window.hpp"

Render* Render::_current=nullptr;

void Render::render(){
  assert(camera!=nullptr);
  _current=this;
  current_projection=camera->getProjection();
  current_view=inverse(camera->getTransform());

  pre_render();

  for(auto it=render_calls.begin();it!=render_calls.end();){
    SafePtr<Renderable> rable = *it;
    if(rable.is_alive()){
      rable->render();
      it++;
    }else{
      it=render_calls.erase(it);
    }
  }

  post_render();
}

mat4 PerspectiveCamera::getProjection(){
  assert(renderer!=nullptr);
  return perspective(fov,(float)renderer->getSize().x/renderer->getSize().y,near,far);
}

mat4 OrthographicCamera::getProjection(){
  assert(renderer!=nullptr);
  float aspect=(float)renderer->getSize().x/renderer->getSize().y;
  return ortho(-size/aspect,size/aspect,-size,size,near,far);
}

void WindowRender::pre_render(){
  glfwMakeContextCurrent(window.glfw());
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void WindowRender::post_render(){
  glfwSwapBuffers(window.glfw());
}

void WindowRender::setSize(uvec2 to){
  window.setSize(to);
}
uvec2 WindowRender::getSize() const {
  return window.getSize();
}


void Render::add(Renderable* rable){
  SafePtr<Renderable> ptr(rable);
  render_calls.push_back(ptr);
}

void Render::remove(Renderable* rable){
  for(auto it=render_calls.begin();it!=render_calls.end();){
    if(!it->is_alive() || it->get_ptr()==rable){
      it=render_calls.erase(it);
    }else{
      it++;
    }
  }
}
