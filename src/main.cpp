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

		//Suzanne sz[3]{};
		//sz[0].transform.translate({-3,0,0});
		//sz[2].transform.translate({3,0,0});

		MeshObject screenplane;
		screenplane.mesh = std::shared_ptr<Mesh>(Mesh::makePlane(fvec3{2,2,0}));
		screenplane.shader = std::shared_ptr<Shader>(new Shader("/passthrough.v.glsl","/debug_material_light.f.glsl"));
		Engine::main_window->viewport->render_cycle[Viewport::LIGHT].add(CALLBACK(screenplane, MeshObject::render));
		screenplane.shader->setUniform("mode_selection",0);

		GLuint face_tex{Texture::readPNG("assets/awesomeface.png")};

		MeshObject ul_plane;
		ul_plane.mesh = std::shared_ptr<Mesh>(Mesh::makePlane(fvec3{2,2,0}));
		ul_plane.shader = std::shared_ptr<Shader>(new Shader("/mesh.v.glsl","/texture_color.f.glsl"));
		Engine::main_window->viewport->render_cycle[Viewport::UNLIT].add(CALLBACK(ul_plane, MeshObject::render));
		ul_plane.bound_uniforms.emplace("color_texture",face_tex);
		//ul_plane.shader->setUniformTexture("color_texture",face_tex);


		ushort* read_img = new ushort[512*512*4]{};
		glBindTexture(GL_TEXTURE_2D, face_tex);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_SHORT, read_img);
		long sum=0;
		for(int n=0;n<512*512*4;n++){
			sum+=read_img[n];
		}
		print("sum of read_img is ",sum);
		print("midpoint of read_img is ",read_img[(256*512+256)*4],", ",read_img[(256*512+256)*4+1],", ",read_img[(256*512+256)*4+2],", ",read_img[(256*512+256)*4+3]);


		//std::shared_ptr<Light> light = std::make_shared<Light>();
		//light->type=Light::POINT;
		//light->position=dvec3(1,1,1);
		//Window::viewport().addLight(light);

		Engine::mainLoop();
	}
	Engine::terminate();
	return 0;
}

