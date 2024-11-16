#include "OrbitCamera.hpp"
#include "core/Input.hpp"
#include "core/Engine.hpp"


void OrbitCamera::update(){

  fvec2 mrel = Input::getMouseVel()*Engine::dT;

  dvec2 theta=mrel*speed;

  dquat yrot=quat(dvec3(0,1,0),-theta.x);

  rotation=quatMul(yrot,rotation);

  dquat xrot=quat(quatRot(rotation,fvec3(1,0,0)),-theta.y);
  rotation=quatMul(xrot,rotation);
}

fmat4 OrbitCamera::getView() const {
  fvec3 offset=quatRot(rotation,fvec3(0,0,orbit_distance));
  Transform tform=transform;
  tform.translate(offset);
  return tform.toInvMatrix();
}

void OrbitCamera::onEscapePress(){
  Engine::quit();
}

void OrbitCamera::init(){
  Input::addKeypressListener(Key::ESCAPE, Callback<>::from<OrbitCamera,&OrbitCamera::onEscapePress>(shared_from_this()));
}
