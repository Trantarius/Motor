#include "OrbitCamera.hpp"
#include "core/Input.hpp"
#include "main.hpp"


void OrbitCamera::update(Updater* upd){

  fvec2 mrel = Main::input->getMouseVel()*upd->dT;

  dvec2 theta=mrel*speed;

  dquat yrot=quat(dvec3(0,1,0),-theta.x);

  rotation=quatMul(yrot,rotation);

  dquat xrot=quat(quatRot(rotation,fvec3(1,0,0)),-theta.y);
  rotation=quatMul(xrot,rotation);
}

fmat4 OrbitCamera::getView(Render*) const {
  fvec3 offset=quatRot(rotation,fvec3(0,0,orbit_distance));
  Transform tform=transform;
  tform.translate(offset);
  return tform.toInvMatrix();
}

void OrbitCamera::onEscapePress(){
  quit();
}

OrbitCamera::OrbitCamera(){
  Main::input->addKeypressListener(Key::ESCAPE,SafeFunc<void()>(this,&OrbitCamera::onEscapePress));
}

OrbitCamera::~OrbitCamera(){
  Main::input->removeKeypressListener(Key::ESCAPE,SafeFunc<void()>(this,&OrbitCamera::onEscapePress));
}
