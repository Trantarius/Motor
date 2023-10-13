#include "Shader.hpp"
#include "main.hpp"
#include "util/io.hpp"
#include "util/gl_enum_names.hpp"

Shader::Shader(string vertex_shader_file,string fragment_shader_file){
  info=new Info();
  info->refcount=1;

  Bloc<char> vertex_source=readfile(vertex_shader_file);
  uint vertex_shader=glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader,1,&vertex_source.ptr,NULL);
  glCompileShader(vertex_shader);
  vertex_source.destroy();
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
    glDeleteShader(vertex_shader);
    delete info;
    info=nullptr;
    throw CompileError(vertex_shader_file+string(": ")+infoLog);
  }

  Bloc<char> fragment_source=readfile(fragment_shader_file);
  uint fragment_shader=glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader,1,&fragment_source.ptr,NULL);
  glCompileShader(fragment_shader);
  fragment_source.destroy();
  // check for shader compile errors
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
    delete info;
    info=nullptr;
    throw CompileError(fragment_shader_file+string(": ")+infoLog);
  }

  info->gl_program=glCreateProgram();
  glAttachShader(info->gl_program,vertex_shader);
  glAttachShader(info->gl_program,fragment_shader);
  glLinkProgram(info->gl_program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  // check for linking errors
  glGetProgramiv(info->gl_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(info->gl_program, 512, NULL, infoLog);
    glDeleteProgram(info->gl_program);
    delete info;
    info=nullptr;
    throw CompileError("linking "+vertex_shader_file+" and "+fragment_shader_file+string(": ")+infoLog);
  }

  int uniform_count;
  glGetProgramiv(info->gl_program,GL_ACTIVE_UNIFORMS,&uniform_count);

  uint tex_unit_counter1=0;
  uint tex_unit_counter2=0;
  uint tex_unit_counter3=0;
  for(int u=0;u<uniform_count;u++){
    UniformInfo uinf;
    char uname[80];

    glGetActiveUniform(info->gl_program,u,80,NULL,&(uinf.size),&(uinf.type),uname);
    assert(uinf.size==1);//arrays not yet supported
    uinf.name=string(uname);
    uinf.id=ID(uinf.name.c_str());
    uinf.location=glGetUniformLocation(info->gl_program,uinf.name.c_str());

    info->uniforms[uinf.id]=uinf;
  }
}

Shader::~Shader(){
  if(info!=nullptr){
    info->refcount--;
    if(info->refcount==0){
      dispose();
    }
  }
}

void Shader::dispose(){
  glDeleteProgram(info->gl_program);
  delete info;
  info=nullptr;
}

bool Shader::isNull() const{
  return info==nullptr;
}

void Shader::operator=(const Shader& s){
  if(info!=nullptr){
    info->refcount--;
    if(info->refcount==0){
      dispose();
    }
  }

  info=s.info;
  if(info!=nullptr){
    info->refcount++;
  }
}

void Shader::use() const {
  assert(info!=nullptr);
  glUseProgram(info->gl_program);
}

#define SET_UNIFORM(utype,ptype,letter,gltype)\
template <> void Shader::setUniform(ID name,utype val){\
  assert(info!=nullptr);\
  assert(info->uniforms.contains(name));\
  UniformInfo* unif=&(info->uniforms[name]);\
  assert(unif->type==gltype);\
  glProgramUniform##letter##v(info->gl_program,unif->location,1,(ptype*)(&val));\
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
template <> void Shader::setUniform(ID name,utype val){\
  assert(info!=nullptr);\
  assert(info->uniforms.contains(name));\
  UniformInfo* unif=&(info->uniforms[name]);\
  assert(unif->type==gltype);\
  glProgramUniformMatrix##letter##v(info->gl_program,unif->location,1,GL_TRUE,(ptype*)(&val));\
  checkGLError();\
}

SET_UNIFORM_MATRIX(fmat2,float,2f,GL_FLOAT_MAT2)
SET_UNIFORM_MATRIX(fmat3,float,3f,GL_FLOAT_MAT3)
SET_UNIFORM_MATRIX(fmat4,float,4f,GL_FLOAT_MAT4)

SET_UNIFORM_MATRIX(dmat2,double,2d,GL_DOUBLE_MAT2)
SET_UNIFORM_MATRIX(dmat3,double,3d,GL_DOUBLE_MAT3)
SET_UNIFORM_MATRIX(dmat4,double,4d,GL_DOUBLE_MAT4)

#undef SET_UNIFORM_MATRIX

template <> void Shader::setUniform(ID name, Texture val){
  assert(info!=nullptr);
  assert(info->uniforms.contains(name));
  UniformInfo* unif=&(info->uniforms[name]);
#ifndef NDEBUG
  if(val.getType()==GL_TEXTURE_1D && unif->type!=GL_SAMPLER_1D ||
     val.getType()==GL_TEXTURE_2D && unif->type!=GL_SAMPLER_2D ||
     val.getType()==GL_TEXTURE_3D && unif->type!=GL_SAMPLER_3D
  ){
    throw std::runtime_error("Wrong texture type, "+tostr(unif->type)+" vs "+tostr(val.getType()));
  }
#endif
  glProgramUniform1i(info->gl_program,unif->location,val.getUnit());
}

Array<Shader::UniformInfo> Shader::getUniforms() const{
  assert(info!=nullptr);
  Array<UniformInfo> unis;
  for(const Pair<ID,UniformInfo>& entry : info->uniforms){
    unis.push_back(entry.second);
  }
  return unis;
}
