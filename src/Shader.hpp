#pragma once
#include <string>
#include <stdexcept>
#include "util/glm.hpp"

using std::string;

class Shader{
  struct Info{
    uint refcount=0;
    uint gl_program=0;
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

  bool is_null() const;

  void use() const;
  template<typename T>
  void setUniform(string name,T val);
};

template <> void Shader::setUniform(string name,float val);
template <> void Shader::setUniform(string name,vec2 val);
template <> void Shader::setUniform(string name,vec3 val);
template <> void Shader::setUniform(string name,vec4 val);

template <> void Shader::setUniform(string name,int val);
template <> void Shader::setUniform(string name,ivec2 val);
template <> void Shader::setUniform(string name,ivec3 val);
template <> void Shader::setUniform(string name,ivec4 val);

template <> void Shader::setUniform(string name,mat2 val);
template <> void Shader::setUniform(string name,mat3 val);
template <> void Shader::setUniform(string name,mat4 val);
