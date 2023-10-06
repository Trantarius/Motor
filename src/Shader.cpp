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

bool Shader::is_null() const{
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

#define SET_UNIFORM(type,ptype,letter)\
template <> void Shader::setUniform(string name,type val){\
  assert(info!=nullptr);\
  int location = glGetUniformLocation(info->gl_program,name.c_str());\
  assert(location!=-1);\
  glProgramUniform##letter##v(info->gl_program,location,1,(ptype*)(&val));\
  checkGLError();\
}

SET_UNIFORM(float,float,1f)
SET_UNIFORM(fvec2,float,2f)
SET_UNIFORM(fvec3,float,3f)
SET_UNIFORM(fvec4,float,4f)

SET_UNIFORM(double,double,1d)
SET_UNIFORM(dvec2,double,2d)
SET_UNIFORM(dvec3,double,3d)
SET_UNIFORM(dvec4,double,4d)

SET_UNIFORM(int,int,1i)
SET_UNIFORM(ivec2,int,2i)
SET_UNIFORM(ivec3,int,3i)
SET_UNIFORM(ivec4,int,4i)

#undef SET_UNIFORM

#define SET_UNIFORM_MATRIX(type,ptype,letter)\
template <> void Shader::setUniform(string name,type val){\
  assert(info!=nullptr);\
  int location = glGetUniformLocation(info->gl_program,name.c_str());\
  assert(location!=-1);\
  glProgramUniformMatrix##letter##v(info->gl_program,location,1,GL_TRUE,(ptype*)(&val));\
  checkGLError();\
}

SET_UNIFORM_MATRIX(fmat2,float,2f)
SET_UNIFORM_MATRIX(fmat3,float,3f)
SET_UNIFORM_MATRIX(fmat4,float,4f)

SET_UNIFORM_MATRIX(dmat2,double,2d)
SET_UNIFORM_MATRIX(dmat3,double,3d)
SET_UNIFORM_MATRIX(dmat4,double,4d)

#undef SET_UNIFORM_MATRIX


#define GET_UNIFORM(type,ptype,letter)\
template <> type Shader::getUniform(string name){\
  assert(info!=nullptr);\
  int location = glGetUniformLocation(info->gl_program,name.c_str());\
  assert(location!=-1);\
  type ret;\
  glGetnUniform##letter##v(info->gl_program,location,sizeof(type),(ptype*)&ret);\
  checkGLError();\
  return ret;\
}

GET_UNIFORM(float,float,f)
GET_UNIFORM(fvec2,float,f)
GET_UNIFORM(fvec3,float,f)
GET_UNIFORM(fvec4,float,f)

GET_UNIFORM(double,double,d)
GET_UNIFORM(dvec2,double,d)
GET_UNIFORM(dvec3,double,d)
GET_UNIFORM(dvec4,double,d)

GET_UNIFORM(int,int,i)
GET_UNIFORM(ivec2,int,i)
GET_UNIFORM(ivec3,int,i)
GET_UNIFORM(ivec4,int,i)

#undef GET_UNIFORM

#define GET_UNIFORM_MATRIX(type,ptype,letter)\
template <> type Shader::getUniform(string name){\
  assert(info!=nullptr);\
  int location = glGetUniformLocation(info->gl_program,name.c_str());\
  assert(location!=-1);\
  type ret;\
  glGetnUniform##letter##v(info->gl_program,location,sizeof(type),(ptype*)&ret);\
  checkGLError();\
  return transpose(ret);\
}

GET_UNIFORM_MATRIX(fmat2,float,f)
GET_UNIFORM_MATRIX(fmat3,float,f)
GET_UNIFORM_MATRIX(fmat4,float,f)

GET_UNIFORM_MATRIX(dmat2,double,d)
GET_UNIFORM_MATRIX(dmat3,double,d)
GET_UNIFORM_MATRIX(dmat4,double,d)

#undef GET_UNIFORM_MATRIX
