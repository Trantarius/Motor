#include "Render.hpp"
#include "Window.hpp"

void Render::pre_cycle(){
  assert(camera!=nullptr);
  current_projection=camera->getProjection();
  current_view=camera->getView();
  pre_render();
}

void Render::post_cycle(){
  post_render();
}

mat4 Camera::getView() const {
  return inverse(getTransform());
}

mat4 PerspectiveCamera::getProjection() const {
  assert(renderer!=nullptr);
  return perspective(fov,(float)renderer->getSize().x/renderer->getSize().y,near,far);
}

mat4 OrthographicCamera::getProjection() const {
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
