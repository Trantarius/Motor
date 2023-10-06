#include "OrbitCamera.hpp"
#include "Input.hpp"

void OrbitCamera::update(Updater* upd){

  vec2 mp = Main::input->getMousePos();
  vec2 mrel = mp-last_mouse_pos;
  last_mouse_pos=mp;

  vec2 theta=mrel*speed;

  mat3 yrot=rotationMtx(vec3(0,1,0),theta.x);

  rotation=yrot*rotation;

  mat3 xrot=rotationMtx(rotation*vec3(1,0,0),theta.y);
  rotation=xrot*rotation;
}

mat4 OrbitCamera::getView() const {
  print();
  vec3 offset=rotation*vec3(0,0,orbit_distance);
  print(offset);
  mat4 tform=getTransform();
  print(tform);
  //mat4 omat=translate(tform,offset);
  tform[3]+=vec4(offset,0);
  print(tform);
  return inverse(tform);
}
