#pragma once
#include <string>
#include <stdexcept>
#include "util/math.hpp"
#include "util/id.hpp"
#include "util/collections.hpp"
#include "util/gl_enum_names.hpp"
#include "Texture.hpp"

using std::string;

class Shader{
public:
  struct UniformInfo{
    ID id;
    string name;
    int location;
    int size;//for arrays
    glEnum type;
  };
private:
  struct Info{
    uint refcount=0;
    uint gl_program=0;
    Map<ID,UniformInfo,ID::Hash> uniforms;
  };
  Info* info=nullptr;
  void dispose();

public:

  struct CompileError : public std::runtime_error{
    CompileError(string what):std::runtime_error(what){}
  };

  Shader(){}
  Shader(const Shader& s){*this=s;}
  Shader(string vertex_shader,string fragment_shader);
  ~Shader();

  void operator=(const Shader& s);

  bool isNull() const;

  void use() const;
  template<typename T>
  void setUniform(ID name,T val);

  Array<UniformInfo> getUniforms() const;
};

STRUCT_TO_STRING(Shader::UniformInfo,id,name,location,size,type)


template <> void Shader::setUniform(ID name,float val);
template <> void Shader::setUniform(ID name,fvec2 val);
template <> void Shader::setUniform(ID name,fvec3 val);
template <> void Shader::setUniform(ID name,fvec4 val);

template <> void Shader::setUniform(ID name,double val);
template <> void Shader::setUniform(ID name,dvec2 val);
template <> void Shader::setUniform(ID name,dvec3 val);
template <> void Shader::setUniform(ID name,dvec4 val);

template <> void Shader::setUniform(ID name,int val);
template <> void Shader::setUniform(ID name,ivec2 val);
template <> void Shader::setUniform(ID name,ivec3 val);
template <> void Shader::setUniform(ID name,ivec4 val);

template <> void Shader::setUniform(ID name,fmat2 val);
template <> void Shader::setUniform(ID name,fmat3 val);
template <> void Shader::setUniform(ID name,fmat4 val);

template <> void Shader::setUniform(ID name,dmat2 val);
template <> void Shader::setUniform(ID name,dmat3 val);
template <> void Shader::setUniform(ID name,dmat4 val);

template <> void Shader::setUniform(ID name,Texture val);
