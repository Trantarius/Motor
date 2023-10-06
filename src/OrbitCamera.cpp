#include "OrbitCamera.hpp"
#include "Input.hpp"

void OrbitCamera::update(Updater* upd){

  fvec2 mp = Main::input->getMousePos();
  fvec2 mrel = mp-last_mouse_pos;
  last_mouse_pos=mp;

  fvec2 theta=mrel*speed;

  fmat3 yrot=rotationMtx(fvec3(0,1,0),theta.x);

  rotation=yrot*rotation;

  fmat3 xrot=rotationMtx(rotation*fvec3(1,0,0),theta.y);
  rotation=xrot*rotation;
}

fmat4 OrbitCamera::getView() const {
  fvec3 offset=rotation*fvec3(0,0,orbit_distance);
  fmat4 tform=getTransform();
  tform=translate(tform,offset);
  return inverse(tform);
}
