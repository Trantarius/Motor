#include "OrbitCamera.hpp"
#include "core/Input.hpp"
#include "core/Engine.hpp"


void OrbitCamera::update(){

	dvec2 theta=Input::getMousePos()*speed;

	dquat yrot=quat(dvec3(0,1,0),-theta.x);

	rotation=quatMul(yrot,quatIdentity<double>());

	dquat xrot=quat(quatRot(rotation,dvec3(1,0,0)),-theta.y);
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
	Input::keypress_listeners[Key::ESCAPE].add(Callback<>::from<OrbitCamera,&OrbitCamera::onEscapePress>(shared_from_this()));
}
