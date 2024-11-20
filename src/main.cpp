#include "core/Engine.hpp"
#include "core/Mesh.hpp"
#include "OrbitCamera.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "core/Input.hpp"
#include "Suzanne.hpp"
#include "util/print.hpp"

int main(){
	Engine::init();
	{
		//Input::print_events=true;
		//Input::setCursorMode(GLFW_CURSOR_DISABLED);



		std::shared_ptr<OrbitCamera> camera = std::make_shared<OrbitCamera>();
		//Window::viewport().camera = camera;

		/*
		Shader shader("/mesh.v.glsl","/lighttest.f.glsl");
		MeshData suzanne = MeshData::readOBJ("assets/suzanne.obj");
		MeshData triangle = MeshData(Bloc<fvec3>(fvec3(-0.5,-0.5,-1),fvec3(0,0.5,-1),fvec3(0.5,-0.5,-1)));

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->mesh_data=suzanne;
		mesh->shader=shader;
		Window::viewport().render_cycle.add(CALLBACK(*mesh,Mesh::render));
		*/

		Suzanne sz[3]{};
		sz[0].transform.translate({-1.5,0,0});
		sz[2].transform.translate({1.5,0,0});

		//std::shared_ptr<Light> light = std::make_shared<Light>();
		//light->type=Light::POINT;
		//light->position=dvec3(1,1,1);
		//Window::viewport().addLight(light);

		Engine::mainLoop();
	}
	Engine::terminate();
	return 0;
}

