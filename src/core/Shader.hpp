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

struct UniformValue{
	union{
		int    int_v;
		ivec2  ivec2_v;
		ivec3  ivec3_v;
		ivec4  ivec4_v;
		float  float_v;
		fvec2  fvec2_v;
		fvec3  fvec3_v;
		fvec4  fvec4_v;
		double double_v;
		dvec2  dvec2_v;
		dvec3  dvec3_v;
		dvec4  dvec4_v;
		fmat2  fmat2_v;
		fmat3  fmat3_v;
		fmat4  fmat4_v;
		dmat2  dmat2_v;
		dmat3  dmat3_v;
		dmat4  dmat4_v;
		GLuint tex_v;
	};
	GLenum type;
	UniformValue(int     v): int_v    (v), type(GL_INT         ) {}
	UniformValue(ivec2   v): ivec2_v  (v), type(GL_INT_VEC2    ) {}
	UniformValue(ivec3   v): ivec3_v  (v), type(GL_INT_VEC3    ) {}
	UniformValue(ivec4   v): ivec4_v  (v), type(GL_INT_VEC4    ) {}
	UniformValue(float   v): float_v  (v), type(GL_FLOAT       ) {}
	UniformValue(fvec2   v): fvec2_v  (v), type(GL_FLOAT_VEC2  ) {}
	UniformValue(fvec3   v): fvec3_v  (v), type(GL_FLOAT_VEC3  ) {}
	UniformValue(fvec4   v): fvec4_v  (v), type(GL_FLOAT_VEC4  ) {}
	UniformValue(double  v): double_v (v), type(GL_DOUBLE      ) {}
	UniformValue(dvec2   v): dvec2_v  (v), type(GL_DOUBLE_VEC2 ) {}
	UniformValue(dvec3   v): dvec3_v  (v), type(GL_DOUBLE_VEC3 ) {}
	UniformValue(dvec4   v): dvec4_v  (v), type(GL_DOUBLE_VEC4 ) {}
	UniformValue(fmat2   v): fmat2_v  (v), type(GL_FLOAT_MAT2  ) {}
	UniformValue(fmat3   v): fmat3_v  (v), type(GL_FLOAT_MAT3  ) {}
	UniformValue(fmat4   v): fmat4_v  (v), type(GL_FLOAT_MAT4  ) {}
	UniformValue(dmat2   v): dmat2_v  (v), type(GL_DOUBLE_MAT2 ) {}
	UniformValue(dmat3   v): dmat3_v  (v), type(GL_DOUBLE_MAT3 ) {}
	UniformValue(dmat4   v): dmat4_v  (v), type(GL_DOUBLE_MAT4 ) {}
	UniformValue(GLuint  v): tex_v    (v), type(GL_SAMPLER     ) {}
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

	void setUniform(const std::string& name,bool val);

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
	void setUniformValue(const std::string& name, const UniformValue& val);
};


