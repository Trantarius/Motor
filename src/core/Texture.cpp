#include "Texture.hpp"
#include <cassert>
#include "defs/gl_defs.hpp"
#include "util/stb_image.h"
#include "util/print.hpp"

GLenum format_from_internal_format(GLenum internal_format){
	switch(internal_format){
		case GL_DEPTH_COMPONENT: return GL_DEPTH_COMPONENT;
		case GL_DEPTH_STENCIL: return GL_DEPTH_STENCIL;
		case GL_RED: return GL_RED;
		case GL_RG: return GL_RG;
		case GL_RGB: return GL_RGB;
		case GL_RGBA: return GL_RGBA;
		case GL_R8: return GL_RED;
		case GL_R8_SNORM: return GL_RED;
		case GL_R16: return GL_RED;
		case GL_R16_SNORM: return GL_RED;
		case GL_RG8: return GL_RG;
		case GL_RG8_SNORM: return GL_RG;
		case GL_RG16: return GL_RG;
		case GL_RG16_SNORM: return GL_RG;
		case GL_R3_G3_B2: return GL_RGB;
		case GL_RGB4: return GL_RGB;
		case GL_RGB5: return GL_RGB;
		case GL_RGB8: return GL_RGB;
		case GL_RGB8_SNORM: return GL_RGB;
		case GL_RGB10: return GL_RGB;
		case GL_RGB12: return GL_RGB;
		case GL_RGB16_SNORM: return GL_RGB;
		case GL_RGBA2: return GL_RGB;
		case GL_RGBA4: return GL_RGB;
		case GL_RGB5_A1: return GL_RGBA;
		case GL_RGBA8: return GL_RGBA;
		case GL_RGBA8_SNORM: return GL_RGBA;
		case GL_RGB10_A2: return GL_RGBA;
		case GL_RGB10_A2UI: return GL_RGBA;
		case GL_RGBA12: return GL_RGBA;
		case GL_RGBA16: return GL_RGBA;
		case GL_SRGB8: return GL_RGB;
		case GL_SRGB8_ALPHA8: return GL_RGBA;
		case GL_R16F: return GL_RED;
		case GL_RG16F: return GL_RG;
		case GL_RGB16F: return GL_RGB;
		case GL_RGBA16F: return GL_RGBA;
		case GL_R32F: return GL_RED;
		case GL_RG32F: return GL_RG;
		case GL_RGB32F: return GL_RGB;
		case GL_RGBA32F: return GL_RGBA;
		case GL_R11F_G11F_B10F: return GL_RGB;
		case GL_RGB9_E5: return GL_RGB;
		case GL_R8I: return GL_RED;
		case GL_R8UI: return GL_RED;
		case GL_R16I: return GL_RED;
		case GL_R16UI: return GL_RED;
		case GL_R32I: return GL_RED;
		case GL_R32UI: return GL_RED;
		case GL_RG8I: return GL_RG;
		case GL_RG8UI: return GL_RG;
		case GL_RG16I: return GL_RG;
		case GL_RG16UI: return GL_RG;
		case GL_RG32I: return GL_RG;
		case GL_RG32UI: return GL_RG;
		case GL_RGB8I: return GL_RGB;
		case GL_RGB8UI: return GL_RGB;
		case GL_RGB16I: return GL_RGB;
		case GL_RGB16UI: return GL_RGB;
		case GL_RGB32I: return GL_RGB;
		case GL_RGB32UI: return GL_RGB;
		case GL_RGBA8I: return GL_RGBA;
		case GL_RGBA8UI: return GL_RGBA;
		case GL_RGBA16I: return GL_RGBA;
		case GL_RGBA16UI: return GL_RGBA;
		case GL_RGBA32I: return GL_RGBA;
		case GL_RGBA32UI: return GL_RGBA;
		case GL_COMPRESSED_RED: return GL_RED;
		case GL_COMPRESSED_RG: return GL_RG;
		case GL_COMPRESSED_RGB: return GL_RGB;
		case GL_COMPRESSED_RGBA: return GL_RGBA;
		case GL_COMPRESSED_SRGB: return GL_RGB;
		case GL_COMPRESSED_SRGB_ALPHA: return GL_RGBA;
		case GL_COMPRESSED_RED_RGTC1: return GL_RED;
		case GL_COMPRESSED_SIGNED_RED_RGTC1: return GL_RED;
		case GL_COMPRESSED_RG_RGTC2: return GL_RG;
		case GL_COMPRESSED_SIGNED_RG_RGTC2: return GL_RG;
		case GL_COMPRESSED_RGBA_BPTC_UNORM: return GL_RGBA;
		case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM: return GL_RGBA;
		case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT: return GL_RGB;
		case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT: return GL_RGB;
		default: return 0;
	}
}

Texture::Texture(GLenum shape, ivec3 size, int layers, int samples, GLenum format, GLenum type, void* data){
	glGenTextures(1,&id);
	this->size = size;
	this->format = format;
	this->layers = layers;
	this->samples = samples;
	this->shape = shape;
	GLenum base_format=format_from_internal_format(format);

	glBindTexture(shape, id);
	glTexParameteri(shape, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(shape, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(shape, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(shape, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(shape, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	switch(shape){
		case GL_TEXTURE_1D:
			glTexImage1D(GL_TEXTURE_1D, 0, format, size.x, 0, base_format, type, data);
			break;
		case GL_TEXTURE_1D_ARRAY:
			glTexImage2D(GL_TEXTURE_1D_ARRAY, 0, format, size.x, layers, 0, base_format, type, data);
			break;
		case GL_TEXTURE_2D:
			glTexImage2D(GL_TEXTURE_2D, 0, format, size.x, size.y, 0, base_format, type, data);
			break;
		case GL_TEXTURE_2D_ARRAY:
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, size.x, size.y, layers, 0, base_format, type, data);
			break;
		case GL_TEXTURE_3D:
			glTexImage3D(GL_TEXTURE_3D, 0, format, size.x, size.y, size.z, 0, base_format, type, data);
			break;
		case GL_TEXTURE_2D_MULTISAMPLE:
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, size.x, size.y, false);
			break;
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			glTexImage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, samples, format, size.x, size.y, layers, false);
			break;
	}
	checkGLError();
}

Texture* Texture::create1D(int size, GLenum format, GLenum type, void* data){
	return new Texture(GL_TEXTURE_1D, {size,0,0}, 0, 0, format, type, data);
}
Texture* Texture::create1DArray(int size, int layers, GLenum format, GLenum type, void* data){
	return new Texture(GL_TEXTURE_1D_ARRAY, {size,0,0}, layers, 0, format, type, data);
}
Texture* Texture::create2D(ivec2 size, GLenum format, GLenum type, void* data){
	return new Texture(GL_TEXTURE_2D, {size, 0}, 0, 0, format, type, data);
}
Texture* Texture::create2DArray(ivec2 size, int layers, GLenum format, GLenum type, void* data){
	return new Texture(GL_TEXTURE_2D_ARRAY, {size,0}, layers, 0, format, type, data);
}
Texture* Texture::create3D(ivec3 size, GLenum format, GLenum type, void* data){
	return new Texture(GL_TEXTURE_3D, size, 0, 0, format, type, data);
}
Texture* Texture::create2DMultisample(ivec2 size, int samples, GLenum format, GLenum type, void* data){
	return new Texture(GL_TEXTURE_2D_MULTISAMPLE, {size, 0}, 0, samples, format, type, data);
}
Texture* Texture::create2DMultisampleArray(ivec2 size, int layers, int samples, GLenum format, GLenum type, void* data){
	return new Texture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, {size, 0}, layers, samples, format, type, data);
}

Texture::~Texture(){
	glDeleteTextures(1,&id);
}

GLuint Texture::getID() const {
	return id;
}

void Texture::setMinFilter(GLenum filter){
	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, filter);
}

GLenum Texture::getMinFilter() const{
	GLenum ret;
	glGetTextureParameterIuiv(id, GL_TEXTURE_MIN_FILTER, &ret);
	return ret;
}

void Texture::setMagFilter(GLenum filter){
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, filter);
}

GLenum Texture::getMagFilter() const{
	GLenum ret;
	glGetTextureParameterIuiv(id, GL_TEXTURE_MAG_FILTER, &ret);
	return ret;
}

void Texture::setWrap(GLenum wrap,int d){
	GLenum dim;
	if(d==1)
		dim = GL_TEXTURE_WRAP_S;
	if(d==2)
		dim = GL_TEXTURE_WRAP_T;
	if(d==3)
		dim = GL_TEXTURE_WRAP_R;
	glTextureParameteri(id, dim, wrap);
}

GLenum Texture::getWrap(int d) const{
	GLenum ret;
	GLenum dim;
	if(d==1)
		dim = GL_TEXTURE_WRAP_S;
	if(d==2)
		dim = GL_TEXTURE_WRAP_T;
	if(d==3)
		dim = GL_TEXTURE_WRAP_R;
	glGetTextureParameterIuiv(id, dim, &ret);
	return ret;
}

int Texture::getSize1D() const{
	assert(getRank()==1);
	return size.x;
}

ivec2 Texture::getSize2D() const{
	assert(getRank()==2);
	return {size.x,size.y};
}

ivec3 Texture::getSize3D() const{
	assert(getRank()==3);
	return size;
}

int Texture::getRank() const{
	if(size.z==0){
		if(size.y==0){
			return 1;
		}
		return 2;
	}
	return 3;
}

GLenum Texture::getFormat() const{
	return format;
}

int Texture::getSamples() const{
	return samples;
}

GLenum Texture::getShape() const{
	return shape;
}

void Texture::bindUnit(int unit) const{
	glBindTextureUnit(unit, id);
}

void Texture::generateMipmaps(){
	glGenerateTextureMipmap(id);
}

int Texture::getMaxCombinedTextureUnits(){
	int i;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &i);
	return i;
}

int Texture::getMaxTextureUnits(){
	int i;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &i);
	return i;
}

GLuint Texture::readPNG(const std::string& path){

	int width=0,height=0,components=0;
	uint16_t* data=stbi_load_16(path.c_str(),&width,&height,&components,0);
	long sum=0;
	for(int n=0;n<width*height*components;n++){
		sum+=data[n];
	}
	print(width, " ", height, " ", components);
	print("sum of PNG image is ",sum);
	print("midpoint of PNG is ",data[(256*512+256)*4],", ",data[(256*512+256)*4+1],", ",data[(256*512+256)*4+2],", ",data[(256*512+256)*4+3]);
	assert(data!=nullptr);
	//Texture* ret = new Texture(GL_TEXTURE_2D, {width,height,0}, 0, 0, GL_RGBA, GL_UNSIGNED_SHORT, data);
	GLuint ret;
	glCreateTextures(GL_TEXTURE_2D, 1, &ret);
	//glTextureStorage2D(ret, 1, GL_RGBA8, width, height);
	glBindTexture(GL_TEXTURE_2D,ret);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data);
	checkGLError();
	stbi_image_free(data);
	return ret;
}
