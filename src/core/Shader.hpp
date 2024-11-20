#pragma once
#include <stdexcept>
#include "util/mat.hpp"
#include "defs/gl_defs.hpp"
#include "util/Bloc.hpp"
#include <map>
#include <list>

struct SubShader{
	GLuint id{0};
	SubShader(GLenum type, const std::string& path);
	~SubShader();
	struct CompileError : public std::runtime_error{
		CompileError(std::string what):std::runtime_error(what){}
	};

	operator bool() const;

	SubShader()=delete;
	SubShader(const SubShader&)=delete;
	SubShader(SubShader&&)=delete;
	SubShader& operator=(const SubShader&)=delete;
	SubShader& operator=(SubShader&&)=delete;
};

class Shader{
public:
	struct Uniform{
		std::string name;
		int index;
		int location;
		int size;//for arrays, 1 if non-array
		int unit{-1};//for textures only
		GLenum type;
	};
private:
	GLuint id=0;
	std::map<std::string,Uniform> uniforms;

public:

	struct CompileError : public std::runtime_error{
		CompileError(std::string what):std::runtime_error(what){}
	};

	Shader(std::string vertex_shader,std::string fragment_shader);
	~Shader();

	operator bool() const;

	Shader()=delete;
	Shader(const Shader&)=delete;
	Shader(Shader&&)=delete;
	Shader& operator=(const Shader&)=delete;
	Shader& operator=(Shader&&)=delete;

	void use() const;

	std::list<Uniform> getUniformList() const;
	Uniform getUniform(const std::string& name) const;
	bool hasUniform(const std::string& name) const;

	// recursively load all shader files (*.glsl) found at path into named strings for #include directives
	// names will be relative to path
	static void loadAllShaderFiles(std::string path);

	void setUniform(const std::string& name,float val);
	void setUniformArray(const std::string& name, Bloc<float> val);
	void setUniform(const std::string& name,fvec2 val);
	void setUniformArray(const std::string& name, Bloc<fvec2> val);
	void setUniform(const std::string& name,fvec3 val);
	void setUniformArray(const std::string& name, Bloc<fvec3> val);
	void setUniform(const std::string& name,fvec4 val);
	void setUniformArray(const std::string& name, Bloc<fvec4> val);

	void setUniform(const std::string& name,double val);
	void setUniformArray(const std::string& name, Bloc<double> val);
	void setUniform(const std::string& name,dvec2 val);
	void setUniformArray(const std::string& name, Bloc<dvec2> val);
	void setUniform(const std::string& name,dvec3 val);
	void setUniformArray(const std::string& name, Bloc<dvec3> val);
	void setUniform(const std::string& name,dvec4 val);
	void setUniformArray(const std::string& name, Bloc<dvec4> val);

	void setUniform(const std::string& name,int val);
	void setUniformArray(const std::string& name, Bloc<int> val);
	void setUniform(const std::string& name,ivec2 val);
	void setUniformArray(const std::string& name, Bloc<ivec2> val);
	void setUniform(const std::string& name,ivec3 val);
	void setUniformArray(const std::string& name, Bloc<ivec3> val);
	void setUniform(const std::string& name,ivec4 val);
	void setUniformArray(const std::string& name, Bloc<ivec4> val);

	void setUniform(const std::string& name,fmat2 val);
	void setUniformArray(const std::string& name, Bloc<fmat2> val);
	void setUniform(const std::string& name,fmat3 val);
	void setUniformArray(const std::string& name, Bloc<fmat3> val);
	void setUniform(const std::string& name,fmat4 val);
	void setUniformArray(const std::string& name, Bloc<fmat4> val);

	void setUniform(const std::string& name,dmat2 val);
	void setUniformArray(const std::string& name, Bloc<dmat2> val);
	void setUniform(const std::string& name,dmat3 val);
	void setUniformArray(const std::string& name, Bloc<dmat3> val);
	void setUniform(const std::string& name,dmat4 val);
	void setUniformArray(const std::string& name, Bloc<dmat4> val);

	void setUniformTexture(const std::string& name,GLuint val);
};
