#pragma once
#include "util/mat.hpp"
#include "Spatial.hpp"
#include "Shader.hpp"
#include <memory>
#include "Texture.hpp"
#include <numbers>
#include "TaskPool.hpp"

struct Camera : public Spatial{
	virtual dmat4 getProjection(ivec2 vp_size) const=0;
	virtual dmat4 getView() const;
};

struct PerspectiveCamera : public Camera{
	double fov=std::numbers::pi/4;
	double near=0.01;
	double far=1000.0;
	dmat4 getProjection(ivec2 vp_size) const override;
};

struct OrthographicCamera : public Camera{
	double size=10.0;
	double near=0.01;
	double far=1000.0;
	dmat4 getProjection(ivec2 vp_size) const override;
};



class Viewport{
public:
	inline static constexpr int MATERIAL_COMPONENT_TEXTURE_COUNT = 4;
	enum MaterialComponent{ALBEDO_TEX, NORMAL_TEX, VERTEX_TEX, RDS_TEX};
private:
	ivec2 size{0,0};
	//textures for material_framebuffer, rds is rougness, diffuse, and specular
	gl::Framebuffer material_framebuffer;
	gl::Texture material_tex_arr;
	gl::Renderbuffer material_depth_renderbuffer;
	gl::Framebuffer framebuffer;
	gl::Renderbuffer depth_renderbuffer;
	gl::Texture output_texture;

	//std::list<std::shared_ptr<Light>> lights;
	//UniformBuffer _light_buffer;
	static void update_material_buffer_size();
	friend class Shader;
public:
	inline static constexpr int RENDER_STAGE_COUNT = 4;
	enum RenderStage{
		NORMAL, LIGHT, UNLIT, UI
	};

	CallbackList<Viewport*> pre_render_cycle;
	CallbackList<const Viewport*> render_cycle[RENDER_STAGE_COUNT];
	dmat4 projection_matrix;
	dmat4 view_matrix;

	Viewport();

	void render();
	ivec2 getSize() const;
	void setSize(ivec2);
	GLuint getMaterialBuffer() const;
	GLuint getOutputTexture() const;
	GLuint getOutputFramebuffer() const;
};
