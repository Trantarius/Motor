#pragma once
#include "core/Mesh.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"

class Suzanne : public MeshObject{
	using MeshObject::mesh;
	using MeshObject::shader;
public:

	Suzanne() {
		mesh = std::shared_ptr<Mesh>(Mesh::readOBJ("assets/suzanne.obj"));
		shader = std::make_shared<Shader>("/mesh.v.glsl","/material.f.glsl");
		Engine::main_window->viewport->render_cycle[Viewport::NORMAL].add(CALLBACK(*this,MeshObject::render));
	}
	~Suzanne(){
		Engine::main_window->viewport->render_cycle[Viewport::NORMAL].remove(CALLBACK(*this,MeshObject::render));
	}
};
