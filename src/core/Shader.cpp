#include "Shader.hpp"
#include <filesystem>
#include "util/io.hpp"
#include "defs/gl_defs.hpp"
#include <cstring>
#include "util/print.hpp"

namespace fs = std::filesystem;

SubShader::SubShader(GLenum type, const std::string& path){
	int source_len;
	glGetNamedStringivARB(path.size(),path.c_str(),GL_NAMED_STRING_LENGTH_ARB,&source_len);
	Bloc<char> source(source_len);
	glGetNamedStringARB(path.size(),path.c_str(),source.size,&source_len,source.ptr);
	checkGLError();

	id=glCreateShader(type);
	glShaderSource(id,1,&source.ptr,NULL);
	glCompileShaderIncludeARB(id,0,NULL,NULL);
	source.destroy();
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		int len=0;
		glGetShaderInfoLog(id, 512, NULL, infoLog);
		glDeleteShader(id);
		id=0;
		throw CompileError(path+string(": ")+infoLog);
	}
	checkGLError();
}
SubShader::~SubShader(){
	if(*this)
		glDeleteShader(id);
}

SubShader::operator bool() const{
	return glIsShader(id);
}

Shader::Shader(string vertex_shader_file,string fragment_shader_file){

	//    Compilation

	SubShader vertex_shader{GL_VERTEX_SHADER, vertex_shader_file};
	SubShader fragment_shader{GL_FRAGMENT_SHADER, fragment_shader_file};

	id=glCreateProgram();
	glAttachShader(id,vertex_shader.id);
	glAttachShader(id,fragment_shader.id);
	glLinkProgram(id);

	// check for linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		glDeleteProgram(id);
		id=0;
		throw CompileError("linking "+vertex_shader_file+" and "+fragment_shader_file+string(": ")+infoLog);
	}
	checkGLError();

	//    Uniform Info Extraction

	int uniform_count;
	glGetProgramiv(id,GL_ACTIVE_UNIFORMS,&uniform_count);
	int texture_unit = 0;

	for(int u=0;u<uniform_count;u++){
		Uniform unif;
		char uname[80];

		unif.index=u;
		glGetActiveUniform(id,u,80,NULL,&(unif.size),&(unif.type),uname);
		unif.name=string(uname);
		unif.location=glGetUniformLocation(id,unif.name.c_str());

		//arrays are often named "xyz[0]", change that to "xyz"
		if(unif.size>1){
			if(unif.name.substr(unif.name.size()-3)=="[0]"){
				unif.name=unif.name.substr(0,unif.name.size()-3);
			}
		}

		switch(unif.type){
			case GL_SAMPLER_1D:
			case GL_SAMPLER_1D_ARRAY:
			case GL_SAMPLER_2D:
			case GL_SAMPLER_2D_ARRAY:
			case GL_SAMPLER_3D:
			case GL_SAMPLER_2D_MULTISAMPLE:
			case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
				unif.unit = texture_unit++;
				glProgramUniform1i(id, unif.location, unif.unit);
		}

		uniforms.emplace(unif.name, unif);
	}
	checkGLError();
}

Shader::~Shader(){
	if(*this)
		glDeleteProgram(id);
}

Shader::operator bool() const{
	return glIsProgram(id);
}


void Shader::use() const {
	assert(*this);
	glUseProgram(id);
}

void Shader::loadAllShaderFiles(string path){
	for(const fs::directory_entry& entry : fs::recursive_directory_iterator(path)){
		fs::path epath=entry.path();
		if(epath.extension()!=".glsl"){
			continue;
		}
		string epath_str=epath;
		epath_str=epath_str.substr(path.size());

		Bloc<char> content=readfile(epath);
		glNamedStringARB(GL_SHADER_INCLUDE_ARB,epath_str.size(),epath_str.c_str(),content.size,content.ptr);
		content.destroy();
	}
	checkGLError();
}


//    Uniform Info Getters

std::list<Shader::Uniform> Shader::getUniformList() const{
	assert(*this);
	std::list<Uniform> unis;
	for(const auto& entry : uniforms){
		unis.push_back(entry.second);
	}
	return unis;
}

Shader::Uniform Shader::getUniform(const std::string& name) const{
	assert(*this);
	return uniforms.at(name);
}

bool Shader::hasUniform(const std::string& name) const{
	assert(*this);
	return uniforms.contains(name);
}

//    Uniform Setters

#define SET_UNIFORM(utype,ptype,letter,gltype)\
void Shader::setUniform(const std::string& name,utype val){\
	assert(*this);\
	assert(hasUniform(name));\
	Uniform& unif = uniforms[name];\
	assert(unif.type==gltype);\
	assert(unif.size==1);\
	glProgramUniform##letter##v(id,unif.location,1,(ptype*)(&val));\
	checkGLError();\
}\
void Shader::setUniformArray(const std::string& name, Bloc<utype> vals){\
	assert(*this);\
	assert(hasUniform(name));\
	Uniform& unif = uniforms[name];\
	assert(unif.type==gltype);\
	assert(unif.size==vals.size);\
	glProgramUniform##letter##v(id,unif.location,unif.size,(ptype*)(vals.ptr));\
	checkGLError();\
}

SET_UNIFORM(float,float,1f,GL_FLOAT)
SET_UNIFORM(fvec2,float,2f,GL_FLOAT_VEC2)
SET_UNIFORM(fvec3,float,3f,GL_FLOAT_VEC3)
SET_UNIFORM(fvec4,float,4f,GL_FLOAT_VEC4)

SET_UNIFORM(double,double,1d,GL_DOUBLE)
SET_UNIFORM(dvec2,double,2d,GL_DOUBLE_VEC2)
SET_UNIFORM(dvec3,double,3d,GL_DOUBLE_VEC3)
SET_UNIFORM(dvec4,double,4d,GL_DOUBLE_VEC4)

SET_UNIFORM(int,int,1i,GL_INT)
SET_UNIFORM(ivec2,int,2i,GL_INT_VEC2)
SET_UNIFORM(ivec3,int,3i,GL_INT_VEC3)
SET_UNIFORM(ivec4,int,4i,GL_INT_VEC4)

#undef SET_UNIFORM

#define SET_UNIFORM_MATRIX(utype,ptype,letter,gltype)\
void Shader::setUniform(const std::string& name,utype val){\
	assert(*this);\
	assert(hasUniform(name));\
	Uniform& unif = uniforms[name];\
	assert(unif.type==gltype);\
	assert(unif.size==1);\
	glProgramUniformMatrix##letter##v(id,unif.location,1,GL_TRUE,(ptype*)(&val));\
	checkGLError();\
}\
void Shader::setUniformArray(const std::string& name, Bloc<utype> vals){\
	assert(*this);\
	assert(hasUniform(name));\
	Uniform& unif = uniforms[name];\
	assert(unif.type==gltype);\
	assert(unif.size==vals.size);\
	glProgramUniformMatrix##letter##v(id,unif.location,unif.size,GL_TRUE,(ptype*)(vals.ptr));\
	checkGLError();\
}

SET_UNIFORM_MATRIX(fmat2,float,2f,GL_FLOAT_MAT2)
SET_UNIFORM_MATRIX(fmat3,float,3f,GL_FLOAT_MAT3)
SET_UNIFORM_MATRIX(fmat4,float,4f,GL_FLOAT_MAT4)

SET_UNIFORM_MATRIX(dmat2,double,2d,GL_DOUBLE_MAT2)
SET_UNIFORM_MATRIX(dmat3,double,3d,GL_DOUBLE_MAT3)
SET_UNIFORM_MATRIX(dmat4,double,4d,GL_DOUBLE_MAT4)

#undef SET_UNIFORM_MATRIX

const std::map<GLenum,GLenum> texture_type_to_sampler_type_map{
{GL_TEXTURE_1D,GL_SAMPLER_1D},
{GL_TEXTURE_1D_ARRAY,GL_SAMPLER_1D_ARRAY},
{GL_TEXTURE_2D,GL_SAMPLER_2D},
{GL_TEXTURE_2D_ARRAY,GL_SAMPLER_2D_ARRAY},
{GL_TEXTURE_3D,GL_SAMPLER_3D},
{GL_TEXTURE_2D_MULTISAMPLE,GL_SAMPLER_2D_MULTISAMPLE},
{GL_TEXTURE_2D_MULTISAMPLE_ARRAY,GL_SAMPLER_2D_MULTISAMPLE_ARRAY},
};

void Shader::setUniformTexture(const std::string& name, GLuint val){
	assert(*this);
	assert(val);
	assert(uniforms.contains(name));
	Uniform& unif=uniforms[name];
	assert(unif.size==1);
#ifndef NDEBUG
	GLenum bind_point;
	glGetTextureParameterIuiv(val, GL_TEXTURE_TARGET, &bind_point);
	assert(unif.type==texture_type_to_sampler_type_map.at(bind_point));
#endif
	glBindTextureUnit(unif.unit, val);
	checkGLError();
}
