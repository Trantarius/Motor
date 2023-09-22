#include "Shader.hpp"
#include "main.hpp"
#include "util/io.hpp"

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

template <> void Shader::setUniform(string name,float val){
  assert(info!=nullptr);
  int location = glGetUniformLocation(info->gl_program,name.c_str());
  assert(location!=-1);
  glProgramUniform1fv(info->gl_program,location,1,(float*)(&val));
}
template <> void Shader::setUniform(string name,vec2 val){
  assert(info!=nullptr);
  int location = glGetUniformLocation(info->gl_program,name.c_str());
  assert(location!=-1);
  glProgramUniform2fv(info->gl_program,location,1,(float*)(&val));
}
template <> void Shader::setUniform(string name,vec3 val){
  assert(info!=nullptr);
  int location = glGetUniformLocation(info->gl_program,name.c_str());
  assert(location!=-1);
  glProgramUniform3fv(info->gl_program,location,1,(float*)(&val));
}
template <> void Shader::setUniform(string name,vec4 val){
  assert(info!=nullptr);
  int location = glGetUniformLocation(info->gl_program,name.c_str());
  assert(location!=-1);
  glProgramUniform4fv(info->gl_program,location,1,(float*)(&val));
}

template <> void Shader::setUniform(string name,int val){
  assert(info!=nullptr);
  int location = glGetUniformLocation(info->gl_program,name.c_str());
  assert(location!=-1);
  glProgramUniform1iv(info->gl_program,location,1,(int*)(&val));
}
template <> void Shader::setUniform(string name,ivec2 val){
  assert(info!=nullptr);
  int location = glGetUniformLocation(info->gl_program,name.c_str());
  assert(location!=-1);
  glProgramUniform2iv(info->gl_program,location,1,(int*)(&val));
}
template <> void Shader::setUniform(string name,ivec3 val){
  assert(info!=nullptr);
  int location = glGetUniformLocation(info->gl_program,name.c_str());
  assert(location!=-1);
  glProgramUniform3iv(info->gl_program,location,1,(int*)(&val));
}
template <> void Shader::setUniform(string name,ivec4 val){
  assert(info!=nullptr);
  int location = glGetUniformLocation(info->gl_program,name.c_str());
  assert(location!=-1);
  glProgramUniform4iv(info->gl_program,location,1,(int*)(&val));
}

template <> void Shader::setUniform(string name,mat2 val){
  assert(info!=nullptr);
  int location = glGetUniformLocation(info->gl_program,name.c_str());
  assert(location!=-1);
  glProgramUniformMatrix2fv(info->gl_program,location,1,false,(float*)(&val));
}
template <> void Shader::setUniform(string name,mat3 val){
  assert(info!=nullptr);
  int location = glGetUniformLocation(info->gl_program,name.c_str());
  assert(location!=-1);
  glProgramUniformMatrix3fv(info->gl_program,location,1,false,(float*)(&val));
}
template <> void Shader::setUniform(string name,mat4 val){
  assert(info!=nullptr);
  int location = glGetUniformLocation(info->gl_program,name.c_str());
  assert(location!=-1);
  glProgramUniformMatrix4fv(info->gl_program,location,1,false,(float*)(&val));
}
