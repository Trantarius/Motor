#include "Render.hpp"
#include "Window.hpp"


void Render::preCycle(){
  assert(camera!=nullptr);
  current_projection=camera->getProjection(this);
  current_view=camera->getView(this);
}

Render::Render(){
  addPreCycle(SafeCall(this,&Render::preCycle));
}

fmat4 Camera::getView(Render* renderer) const {
  return inverse(getTransform());
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
