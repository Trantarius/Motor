#include "OrbitCamera.hpp"
#include "core/Input.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"


void OrbitCamera::update(Viewport* vp){

	dvec2 theta=Input::getMousePos()*speed;

	dquat yrot=quat(dvec3(0,1,0),-theta.x);

	rotation=quatMul(yrot,quatIdentity<double>());

	dquat xrot=quat(quatRot(rotation,dvec3(1,0,0)),-theta.y);
	rotation=quatMul(xrot,rotation);

	vp->projection_matrix = getProjection(vp->getSize());
	vp->view_matrix = getView();
}

dmat4 OrbitCamera::getView() const {
	fvec3 offset=quatRot(rotation,fvec3(0,0,orbit_distance));
	Transform tform=transform;
	tform.translate(offset);
	return tform.toInvMatrix();
}

void OrbitCamera::onEscapePress(){
	Engine::quit();
}

OrbitCamera::OrbitCamera(){
	Engine::main_window->viewport->pre_render_cycle.add(CALLBACK(*this,OrbitCamera::update));
	Input::keypress_listeners[Key::ESCAPE].add(CALLBACK(*this,OrbitCamera::onEscapePress));
}

OrbitCamera::~OrbitCamera(){
	Engine::main_window->viewport->pre_render_cycle.remove(CALLBACK(*this,OrbitCamera::update));
	Input::keypress_listeners[Key::ESCAPE].remove(CALLBACK(*this,OrbitCamera::onEscapePress));
}
