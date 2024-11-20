#include "Render.hpp"
#include "Window.hpp"


ivec2 Viewport::getSize() const{
	return size;
}

void Viewport::setSize(ivec2 to){
	size = to;
	GLenum draw_buffers[4]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};

	glBindTexture(GL_TEXTURE_2D_ARRAY,material_tex_arr);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, size.x, size.y, 4, 0, GL_RGB, GL_INT, NULL);
	glBindRenderbuffer(GL_RENDERBUFFER, material_depth_renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
	glBindFramebuffer(GL_FRAMEBUFFER, material_framebuffer);
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, material_tex_arr, 0, 0);
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, material_tex_arr, 0, 1);
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, material_tex_arr, 0, 2);
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, material_tex_arr, 0, 3);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, material_depth_renderbuffer);
	glDrawBuffers(4,draw_buffers);
	checkGLError();

	glBindRenderbuffer(GL_RENDERBUFFER, depth_renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
	glBindTexture(GL_TEXTURE_2D,output_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_INT, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, output_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_renderbuffer);
	glDrawBuffers(1,draw_buffers);
	checkGLError();
}

GLuint Viewport::getOutputTexture(){
	return output_texture.id;
}

GLuint Viewport::getOutputFramebuffer(){
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,size.x,size.y);
	checkGLError();

	render_cycle[NORMAL].dumpInto(pool, this);
	pool.flush();
	checkGLError();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,framebuffer);
	glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0,0,size.x,size.y);
	glClear(GL_COLOR_BUFFER_BIT);
	checkGLError();

	render_cycle[LIGHT].dumpInto(pool, this);
	pool.flush();
	checkGLError();

	render_cycle[UNLIT].dumpInto(pool, this);
	pool.flush();
	checkGLError();

	glClear(GL_DEPTH_BUFFER_BIT);

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
