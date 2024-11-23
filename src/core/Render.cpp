#include "Render.hpp"
#include "Window.hpp"
#include "defs/gl_defs.hpp"


ivec2 Viewport::getSize() const{
	return size;
}

Viewport::Viewport(){
	glBindTexture(GL_TEXTURE_2D_ARRAY, material_tex_arr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glBindTexture(GL_TEXTURE_2D, output_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Viewport::setSize(ivec2 to){
	size = to;

	static_assert(MATERIAL_COMPONENT_TEXTURE_COUNT<=4); // if this fails, make draw_buffers bigger
	GLenum draw_buffers[4]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};

	glBindFramebuffer(GL_FRAMEBUFFER, material_framebuffer);

	glBindTexture(GL_TEXTURE_2D_ARRAY, material_tex_arr);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, size.x, size.y, MATERIAL_COMPONENT_TEXTURE_COUNT, 0, GL_RGB, GL_INT, NULL);
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, material_tex_arr, 0, 0);
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, material_tex_arr, 0, 1);
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, material_tex_arr, 0, 2);
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, material_tex_arr, 0, 3);

	glBindRenderbuffer(GL_RENDERBUFFER, material_depth_renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, material_depth_renderbuffer);
	glDrawBuffers(MATERIAL_COMPONENT_TEXTURE_COUNT,draw_buffers);
	checkGLError();

	glBindRenderbuffer(GL_RENDERBUFFER, depth_renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
	glBindTexture(GL_TEXTURE_2D,output_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_INT, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, output_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_renderbuffer);
	glDrawBuffers(1,draw_buffers);
	checkGLError();
}

GLuint Viewport::getMaterialBuffer() const{
	return material_tex_arr;
}
GLuint Viewport::getOutputTexture() const{
	return output_texture.id;
}

GLuint Viewport::getOutputFramebuffer() const{
	return framebuffer.id;
}

void Viewport::render(){

	if(size.x<=0 || size.y<=0){
		setSize(ivec2(1024,600));
	}

	TaskPool pool;
	pre_render_cycle.dumpInto(pool, this);
	pool.flush();
	checkGLError();

	glBindFramebuffer(GL_FRAMEBUFFER,material_framebuffer);
	static_assert(MATERIAL_COMPONENT_TEXTURE_COUNT<=4); // if this fails, make draw_buffers bigger
	GLenum draw_buffers[4]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(MATERIAL_COMPONENT_TEXTURE_COUNT,draw_buffers);
	glViewport(0,0,size.x,size.y);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE); //enable writing to depth buffer
	glDisable(GL_BLEND);
	//glClearColor(0,0,0.5,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2,0.2,0.2,1);
	checkGLError();

	render_cycle[NORMAL].dumpInto(pool, this);
	pool.flush();
	checkGLError();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, material_framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,framebuffer);
	glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glDrawBuffers(1,draw_buffers);
	glViewport(0,0,size.x,size.y);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE); //disable writing to depth buffer
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE); // additive blending
	glClear(GL_COLOR_BUFFER_BIT);
	checkGLError();
	//glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	render_cycle[LIGHT].dumpInto(pool, this);
	pool.flush();
	checkGLError();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	render_cycle[UNLIT].dumpInto(pool, this);
	pool.flush();
	checkGLError();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // standard alpha blending

	render_cycle[UI].dumpInto(pool, this);
	pool.flush();
	checkGLError();
}

dmat4 Camera::getView() const {
	return transform.toInvMatrix();
}

dmat4 PerspectiveCamera::getProjection(ivec2 vp_size) const {
	//http://www.songho.ca/opengl/gl_projectionmatrix.html
	double height=tan(fov/2)*near;
	double width=height * (double)vp_size.x/vp_size.y;
	return dmat4(
		near/width,   0,    0,    0,
		0,    near/height,    0,    0,
		0,    0,    (far+near)/(near-far), 2*far*near/(near-far),
		0,    0,    -1,   0
	);
}

dmat4 OrthographicCamera::getProjection(ivec2 vp_size) const {
	double aspect=(double)vp_size.x/vp_size.y;
	double height=size;
	double width=height*aspect;
	return dmat4(
		1/width,    0,    0,    0,
		0,    1/height,   0,    0,
		0,    0,    2/(near-far),   (near+far)/(near-far),
		0,    0,    0,    1
	);
}
