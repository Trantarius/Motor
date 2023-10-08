#include "OrbitCamera.hpp"
#include "Input.hpp"


fmat3 last_rot;

void OrbitCamera::update(Updater* upd){

  fvec2 lmp=last_mouse_pos;
  fvec2 mp = Main::input->getMousePos();
  fvec2 mrel = mp-last_mouse_pos;
  last_mouse_pos=mp;

  fvec2 theta=mrel*speed;

  fmat3 yrot=rotationMtx(fvec3(0,1,0),theta.x);

  rotation=yrot*rotation;

  fmat3 xrot=rotationMtx(rotation*fvec3(1,0,0),theta.y);
  rotation=xrot*rotation;

  if(len(mrel)>100){
    print(mrel,": ",lmp,"->",mp);
  }

  last_rot=rotation;
}

fmat4 OrbitCamera::getView(Render*) const {
  fvec3 offset=rotation*fvec3(0,0,orbit_distance);
  fmat4 tform=getTransform();
  tform=translate(tform,offset);
  return inverse(tform);
}

void OrbitCamera::onEscapePress(){
  quit();
}

OrbitCamera::OrbitCamera(){
  Main::input->addKeypressListener(Key::ESCAPE,SafeCall(this,&OrbitCamera::onEscapePress));
}

OrbitCamera::~OrbitCamera(){
  Main::input->removeKeypressListener(Key::ESCAPE,SafeCall(this,&OrbitCamera::onEscapePress));
}
