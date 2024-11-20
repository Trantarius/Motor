#pragma once
#include <cstdlib>
#include "util/vec.hpp"
#include "defs/gl_defs.hpp"

class Texture{
	GLuint id{0};
	ivec3 size{0,0,0};
	int layers{0};//for *_ARRAY types only; 0 for all others
	GLenum format{0};
	int samples{0};
	// one of: GL_TEXTURE_1D, GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY,
	// GL_TEXTURE_3D, GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_2D_MULTISAMPLE_ARRAY
	GLenum shape{0};

	//Texture();
	// creates a new texture from the given image (or an empty one if data is nullptr)
	// shape may be one of: GL_TEXTURE_1D, GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_3D, GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_2D_MULTISAMPLE_ARRAY
	// unused values of size must be 0
	// layers must be 0 for non-array types, non-zero for array types
	// format determines the output texture's pixel format (see internalFormat of https://docs.gl/gl4/glTexImage2D)
	// type determines the type of the data found in data
	// data is the image to place in the texture, or null
	Texture(GLenum shape, ivec3 size, int layers, int samples, GLenum format, GLenum type, void* data);

public:

	// creates a new texture from the given image
	// format determines the output texture's pixel format
	// type determines the type of the data found in data
	// data is the image to place in the texture, or null
	static Texture* create1D(int size, GLenum format, GLenum type, void* data);
	static Texture* create1DArray(int size, int layers, GLenum format, GLenum type, void* data);
	static Texture* create2D(ivec2 size, GLenum format, GLenum type, void* data);
	static Texture* create2DArray(ivec2 size, int layers, GLenum format, GLenum type, void* data);
	static Texture* create3D(ivec3 size, GLenum format, GLenum type, void* data);
	static Texture* create2DMultisample(ivec2 size, int samples, GLenum format, GLenum type, void* data);
	static Texture* create2DMultisampleArray(ivec2 size, int layers, int samples, GLenum format, GLenum type, void* data);

	~Texture();

	Texture()=delete;
	Texture(const Texture&)=delete;
	Texture(Texture&&)=delete;
	Texture& operator=(const Texture&)=delete;
	Texture& operator=(Texture&&)=delete;

	GLuint getID() const;
	// options: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST,
	// GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR
	void setMinFilter(GLenum filter);
	GLenum getMinFilter() const;
	// options: GL_NEAREST, GL_LINEAR
	void setMagFilter(GLenum filter);
	GLenum getMagFilter() const;
	// options: GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT, GL_REPEAT,
	// GL_MIRROR_CLAMP_TO_EDGE
	void setWrap(GLenum wrap,int d);
	GLenum getWrap(int) const;

	int getSize1D() const;
	ivec2 getSize2D() const;
	ivec3 getSize3D() const;
	// dimensions per layer
	int getRank() const;
	// 0 for non-array textures
	int getLayers() const;

	GLenum getFormat() const;
	int getSamples() const;
	GLenum getShape() const;

	void bindUnit(int) const;
	void generateMipmaps();

	static Texture* readPNG(const std::string& path);
	static int getMaxCombinedTextureUnits();
	static int getMaxTextureUnits();
};
