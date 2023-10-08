#pragma once
#include <string>
#include <stdexcept>
#include "util/math.hpp"

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

  bool isNull() const;

  void use() const;
  template<typename T>
  void setUniform(string name,T val);
  template<typename T>
  T getUniform(string name);
};

template <> void Shader::setUniform(string name,float val);
template <> void Shader::setUniform(string name,fvec2 val);
template <> void Shader::setUniform(string name,fvec3 val);
template <> void Shader::setUniform(string name,fvec4 val);

template <> void Shader::setUniform(string name,double val);
template <> void Shader::setUniform(string name,dvec2 val);
template <> void Shader::setUniform(string name,dvec3 val);
template <> void Shader::setUniform(string name,dvec4 val);

template <> void Shader::setUniform(string name,int val);
template <> void Shader::setUniform(string name,ivec2 val);
template <> void Shader::setUniform(string name,ivec3 val);
template <> void Shader::setUniform(string name,ivec4 val);

template <> void Shader::setUniform(string name,fmat2 val);
template <> void Shader::setUniform(string name,fmat3 val);
template <> void Shader::setUniform(string name,fmat4 val);

template <> void Shader::setUniform(string name,dmat2 val);
template <> void Shader::setUniform(string name,dmat3 val);
template <> void Shader::setUniform(string name,dmat4 val);



template <> float Shader::getUniform(string name);
template <> fvec2 Shader::getUniform(string name);
template <> fvec3 Shader::getUniform(string name);
template <> fvec4 Shader::getUniform(string name);

template <> double Shader::getUniform(string name);
template <> dvec2 Shader::getUniform(string name);
template <> dvec3 Shader::getUniform(string name);
template <> dvec4 Shader::getUniform(string name);

template <> int Shader::getUniform(string name);
template <> ivec2 Shader::getUniform(string name);
template <> ivec3 Shader::getUniform(string name);
template <> ivec4 Shader::getUniform(string name);

template <> fmat2 Shader::getUniform(string name);
template <> fmat3 Shader::getUniform(string name);
template <> fmat4 Shader::getUniform(string name);

template <> dmat2 Shader::getUniform(string name);
template <> dmat3 Shader::getUniform(string name);
template <> dmat4 Shader::getUniform(string name);
