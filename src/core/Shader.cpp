#include "Shader.hpp"
#include <filesystem>
#include "util/io.hpp"
#include "defs/gl_defs.hpp"
#include "Texture.hpp"

namespace fs = std::filesystem;

Shader::Shader(string vertex_shader_file,string fragment_shader_file):Shader(_ref_count_init()){

  //    Compilation

  int vs_len;
  glGetNamedStringivARB(vertex_shader_file.size(),vertex_shader_file.c_str(),GL_NAMED_STRING_LENGTH_ARB,&vs_len);
  checkGLError();
  Bloc<char> vertex_source(vs_len);
  glGetNamedStringARB(vertex_shader_file.size(),vertex_shader_file.c_str(),vertex_source.size,&vs_len,vertex_source.ptr);

  uint vertex_shader=glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader,1,&vertex_source.ptr,NULL);
  glCompileShaderIncludeARB(vertex_shader,0,NULL,NULL);
  //glCompileShader(vertex_shader);
  vertex_source.destroy();
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    int len=0;
    glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
    glDeleteShader(vertex_shader);
    delete data;
    data=nullptr;
    throw CompileError(vertex_shader_file+string(": ")+infoLog);
  }

  int fs_len;
  glGetNamedStringivARB(fragment_shader_file.size(),fragment_shader_file.c_str(),GL_NAMED_STRING_LENGTH_ARB,&fs_len);
  checkGLError();
  Bloc<char> fragment_source(fs_len);
  glGetNamedStringARB(fragment_shader_file.size(),fragment_shader_file.c_str(),fragment_source.size,&fs_len,fragment_source.ptr);

  uint fragment_shader=glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader,1,&fragment_source.ptr,NULL);
  //glCompileShader(fragment_shader);
  glCompileShaderIncludeARB(fragment_shader,0,NULL,NULL);
  fragment_source.destroy();
  // check for shader compile errors
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glDeleteShader(vertex_shader);
    glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
    glDeleteShader(fragment_shader);
    delete data;
    data=nullptr;
    throw CompileError(fragment_shader_file+string(": ")+infoLog);
  }

  data->gl_program=glCreateProgram();
  glAttachShader(data->gl_program,vertex_shader);
  glAttachShader(data->gl_program,fragment_shader);
  glLinkProgram(data->gl_program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  // check for linking errors
  glGetProgramiv(data->gl_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(data->gl_program, 512, NULL, infoLog);
    glDeleteProgram(data->gl_program);
    delete data;
    data=nullptr;
    throw CompileError("linking "+vertex_shader_file+" and "+fragment_shader_file+string(": ")+infoLog);
  }



  //    Uniform Info Extraction

  int uniform_count;
  glGetProgramiv(data->gl_program,GL_ACTIVE_UNIFORMS,&uniform_count);
  bool* uniform_checklist=new bool[uniform_count]{false};

  //    Uniform Blocks

  int block_count;
  glGetProgramiv(data->gl_program,GL_ACTIVE_UNIFORM_BLOCKS,&block_count);

  for(int block_idx=0; block_idx<block_count; block_idx++){
    UniformBlock block;
    char bname[80];
    glGetActiveUniformBlockName(data->gl_program,block_idx,80,NULL,bname);
    block.name=string(bname);
    block.id=ID(block.name.c_str());
    block.index=block_idx;
    glGetActiveUniformBlockiv(data->gl_program,block_idx,GL_UNIFORM_BLOCK_DATA_SIZE,&(block.data_size));

    int member_count;
    glGetActiveUniformBlockiv(data->gl_program,block_idx,GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,&member_count);
    int* member_indices=new int[member_count];
    glGetActiveUniformBlockiv(data->gl_program,block_idx,GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,member_indices);

    for(int m=0;m<member_count;m++){
      uint member_idx=member_indices[m];
      UniformBlockMember member;

      glGetActiveUniform(data->gl_program,member_idx,80,NULL,&member.size,&member.type,bname);
      member.name=string(bname);
      member.id=ID(member.name.c_str());
      member.index=member_idx;

      //arrays are often named "xyz[0]", change that to "xyz"
      if(member.size>1){
        if(member.name.substr(member.name.size()-3)=="[0]"){
          member.name=member.name.substr(0,member.name.size()-3);
          member.id=ID(member.name.c_str());
        }
      }

      glGetActiveUniformsiv(data->gl_program,1,&member_idx,GL_UNIFORM_OFFSET,&member.offset);
      glGetActiveUniformsiv(data->gl_program,1,&member_idx,GL_UNIFORM_ARRAY_STRIDE,&member.array_stride);
      glGetActiveUniformsiv(data->gl_program,1,&member_idx,GL_UNIFORM_MATRIX_STRIDE,&member.matrix_stride);

      block.members[member.id]=member;
      uniform_checklist[member_idx]=true;
    }

    delete [] member_indices;
    data->uniform_blocks[block.id]=block;
  }

  //    Non-Block Uniforms

  for(int u=0;u<uniform_count;u++){
    if(uniform_checklist[u]){
      continue;//uniform was part of block
    }
    UniformInfo uinf;
    char uname[80];

    uinf.index=u;
    glGetActiveUniform(data->gl_program,u,80,NULL,&(uinf.size),&(uinf.type),uname);
    uinf.name=string(uname);
    uinf.id=ID(uinf.name.c_str());
    uinf.location=glGetUniformLocation(data->gl_program,uinf.name.c_str());

    //arrays are often named "xyz[0]", change that to "xyz"
    if(uinf.size>1){
      if(uinf.name.substr(uinf.name.size()-3)=="[0]"){
        uinf.name=uinf.name.substr(0,uinf.name.size()-3);
        uinf.id=ID(uinf.name.c_str());
      }
    }

    data->uniforms[uinf.id]=uinf;
  }
  delete [] uniform_checklist;
  checkGLError();
}

void Shader::onDestroy(){
  glDeleteProgram(data->gl_program);
}

void Shader::use() const {
  assert(data!=nullptr);
  glUseProgram(data->gl_program);
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

void Shader::setUniformBlock(ID name, UniformBuffer buffer){
  assert(data!=nullptr);
  assert(buffer.data!=nullptr);
  assert(data->uniform_blocks.contains(name));
  UniformBlock& block=data->uniform_blocks[name];
  assert(buffer.data->block.id==block.id);
  glUniformBlockBinding(data->gl_program,block.index,buffer.data->binding);
  checkGLError();
}



//    Uniform Info Getters

std::vector<UniformInfo> Shader::getUniforms() const{
  assert(data!=nullptr);
  std::vector<UniformInfo> unis;
  for(const std::pair<ID,UniformInfo>& entry : data->uniforms){
    unis.push_back(entry.second);
  }
  return unis;
}

UniformInfo Shader::getUniform(ID id) const{
  assert(data!=nullptr);
  assert(hasUniform(id));
  return data->uniforms[id];
}

bool Shader::hasUniform(ID id) const{
  assert(data!=nullptr);
  return data->uniforms.contains(id);
}

std::vector<UniformBlock> Shader::getUniformBlocks() const{
  assert(data!=nullptr);
  std::vector<UniformBlock> blocks;
  for(const std::pair<ID,UniformBlock>& entry : data->uniform_blocks){
    blocks.push_back(entry.second);
  }
  return blocks;
}

UniformBlock Shader::getUniformBlock(ID id) const{
  assert(data!=nullptr);
  assert(hasUniformBlock(id));
  return data->uniform_blocks[id];
}

bool Shader::hasUniformBlock(ID id) const{
  assert(data!=nullptr);
  return data->uniform_blocks.contains(id);
}



//    Uniform Setters

#define SET_UNIFORM(utype,ptype,letter,gltype)\
template <> void Shader::setUniform(ID name,utype val){\
  assert(data!=nullptr);\
  assert(data->uniforms.contains(name));\
  UniformInfo* unif=&(data->uniforms[name]);\
  assert(unif->type==gltype);\
  assert(unif->size==1);\
  glProgramUniform##letter##v(data->gl_program,unif->location,1,(ptype*)(&val));\
  checkGLError();\
}\
template <> void Shader::setUniformArray(ID name,const Bloc<utype> vals){\
  assert(data!=nullptr);\
  assert(data->uniforms.contains(name));\
  UniformInfo* unif=&(data->uniforms[name]);\
  assert(unif->type==gltype);\
  assert(vals.size==unif->size);\
  glProgramUniform##letter##v(data->gl_program,unif->location,unif->size,(ptype*)(vals.ptr));\
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
  assert(data!=nullptr);\
  assert(data->uniforms.contains(name));\
  UniformInfo* unif=&(data->uniforms[name]);\
  assert(unif->type==gltype);\
  assert(unif->size==1);\
  glProgramUniformMatrix##letter##v(data->gl_program,unif->location,1,GL_TRUE,(ptype*)(&val));\
  checkGLError();\
}\
template <> void Shader::setUniformArray(ID name,const Bloc<utype> vals){\
  assert(data!=nullptr);\
  assert(data->uniforms.contains(name));\
  UniformInfo* unif=&(data->uniforms[name]);\
  assert(unif->type==gltype);\
  assert(vals.size==unif->size);\
  glProgramUniformMatrix##letter##v(data->gl_program,unif->location,unif->size,GL_TRUE,(ptype*)(vals.ptr));\
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
  assert(data!=nullptr);
  assert(data->uniforms.contains(name));
  UniformInfo* unif=&(data->uniforms[name]);
  assert(unif->size==1);
#ifndef NDEBUG
  if(val.getType()==GL_TEXTURE_1D && unif->type!=GL_SAMPLER_1D ||
     val.getType()==GL_TEXTURE_2D && unif->type!=GL_SAMPLER_2D ||
     val.getType()==GL_TEXTURE_3D && unif->type!=GL_SAMPLER_3D
  ){
    throw std::runtime_error("Wrong texture type, "+tostr(unif->type)+" vs "+tostr(val.getType()));
  }
  else if(
    unif->type!=GL_SAMPLER_1D &&
    unif->type!=GL_SAMPLER_2D &&
    unif->type!=GL_SAMPLER_3D
  ){
    throw std::runtime_error("Wrong uniform type, "+tostr(unif->type)+" vs "+tostr(val.getType()));
  }
#endif
  glProgramUniform1i(data->gl_program,unif->location,val.getUnit());
  checkGLError();
}

template <> void Shader::setUniformArray(ID name, const Bloc<Texture> vals){
  assert(data!=nullptr);
  assert(data->uniforms.contains(name));
  UniformInfo* unif=&(data->uniforms[name]);
  assert(vals.size==unif->size);

  int* units=new int[unif->size]{};
  for(int n=0;n<unif->size;n++){
    units[n]=vals[n].getUnit();
#ifndef NDEBUG
    if(vals[n].getType()==GL_TEXTURE_1D && unif->type!=GL_SAMPLER_1D ||
      vals[n].getType()==GL_TEXTURE_2D && unif->type!=GL_SAMPLER_2D ||
      vals[n].getType()==GL_TEXTURE_3D && unif->type!=GL_SAMPLER_3D
    ){
      delete [] units;
      throw std::runtime_error("Wrong texture type, "+tostr(unif->type)+" vs "+tostr(vals[n].getType()));
    }
    else if(
      unif->type!=GL_SAMPLER_1D &&
      unif->type!=GL_SAMPLER_2D &&
      unif->type!=GL_SAMPLER_3D
    ){
      delete [] units;
      throw std::runtime_error("Wrong uniform type, "+tostr(unif->type)+" vs "+tostr(vals[n].getType()));
    }
#endif
  }

  glProgramUniform1iv(data->gl_program,unif->location,unif->size,units);
  delete [] units;
  checkGLError();
}



//    Uniform Buffer

IndexDistributor UniformBuffer::binding_distributor(36);//OpenGL spec demands at least 36

UniformBuffer::UniformBuffer(const UniformBlock& block):UniformBuffer(_ref_count_init()){
  data->block=block;
  glCreateBuffers(1,&(data->buffer));
  checkGLError();
  glNamedBufferData(data->buffer,block.data_size,NULL,GL_DYNAMIC_DRAW);
  checkGLError();
  data->binding=binding_distributor.takeIndex();
  if(data->binding<0){
    throw std::runtime_error("Ran out of uniform buffers");
  }
  glBindBufferBase(GL_UNIFORM_BUFFER,data->binding,data->buffer);
  checkGLError();
}

void UniformBuffer::beginUpdate(){
  assert(data!=nullptr);
  assert(data->update_data==nullptr);

  data->update_data.ptr = new uint8_t[data->block.data_size];
  data->update_data.size=data->block.data_size;
}

void UniformBuffer::endUpdate(){
  assert(data!=nullptr);
  assert(data->update_data!=nullptr);

  glNamedBufferData(data->buffer,data->update_data.size,data->update_data.ptr,GL_DYNAMIC_DRAW);
  data->update_data.destroy();
  checkGLError();
}

UniformBlock UniformBuffer::getUniformBlock() const{
  assert(data!=nullptr);
  return data->block;
}

void UniformBuffer::onDestroy(){
  if(data->update_data!=nullptr){
    data->update_data.destroy();
  }
  glDeleteBuffers(1,&(data->buffer));
  binding_distributor.releaseIndex(data->binding);
}

#define SET_UNIFORM(utype,gltype)\
template <> void UniformBuffer::setUniform(ID name,utype val){\
  assert(data!=nullptr);\
  assert(data->update_data!=nullptr);\
  assert(data->block.members.contains(name));\
  UniformBlockMember member=data->block.members[name];\
  assert(member.size==1);\
  assert(member.type==gltype);\
  memcpy(data->update_data+member.offset,&val,sizeof(utype));\
}\
\
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<utype> vals){\
  assert(data!=nullptr);\
  assert(data->update_data!=nullptr);\
  assert(data->block.members.contains(name));\
  UniformBlockMember member=data->block.members[name];\
  assert(member.size==vals.size);\
  assert(member.type==gltype);\
\
  Bloc target=data->update_data+member.offset;\
  for(int n=0;n<vals.size;n++){\
    memcpy(target,&(vals[n]),sizeof(utype));\
    target=target+member.array_stride;\
  }\
}

SET_UNIFORM(float,GL_FLOAT)
SET_UNIFORM(fvec2,GL_FLOAT_VEC2)
SET_UNIFORM(fvec3,GL_FLOAT_VEC3)
SET_UNIFORM(fvec4,GL_FLOAT_VEC4)

SET_UNIFORM(int,GL_INT)
SET_UNIFORM(ivec2,GL_INT_VEC2)
SET_UNIFORM(ivec3,GL_INT_VEC3)
SET_UNIFORM(ivec4,GL_INT_VEC4)

SET_UNIFORM(double,GL_DOUBLE)
SET_UNIFORM(dvec2,GL_DOUBLE_VEC2)
SET_UNIFORM(dvec3,GL_DOUBLE_VEC3)
SET_UNIFORM(dvec4,GL_DOUBLE_VEC4)

#undef SET_UNIFORM

#define SET_UNIFORM_MATRIX(utype,gltype,M)\
template <> void UniformBuffer::setUniform(ID name,utype##M val){\
  assert(data!=nullptr);\
  assert(data->update_data!=nullptr);\
  assert(data->block.members.contains(name));\
  UniformBlockMember member=data->block.members[name];\
  assert(member.size==1);\
  assert(member.type==gltype##M);\
  \
  Bloc target=data->update_data+member.offset;\
  for(int c=0;c<M;c++){\
    Bloc col_target=target;\
    for(int r=0;r<M;r++){\
      auto e = val[r][c];\
      memcpy(col_target,&e,sizeof(e));\
      col_target=col_target+sizeof(e);\
    }\
    target=target+member.matrix_stride;\
  }\
}\
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<utype##M> vals){\
  assert(data!=nullptr);\
  assert(data->update_data!=nullptr);\
  assert(data->block.members.contains(name));\
  UniformBlockMember member=data->block.members[name];\
  assert(member.size==vals.size);\
  assert(member.type==gltype##M);\
  \
  Bloc target=data->update_data+member.offset;\
  for(int n=0;n<vals.size;n++){\
    Bloc mat_target=target;\
    for(int c=0;c<M;c++){\
      Bloc col_target=mat_target;\
      for(int r=0;r<M;r++){\
        auto e = vals[n][r][c];\
        memcpy(col_target,&e,sizeof(e));\
        col_target=col_target+sizeof(e);\
      }\
      mat_target=mat_target+member.matrix_stride;\
    }\
    target=target+member.array_stride;\
  }\
}

SET_UNIFORM_MATRIX(fmat,GL_FLOAT_MAT,2)
SET_UNIFORM_MATRIX(fmat,GL_FLOAT_MAT,3)
SET_UNIFORM_MATRIX(fmat,GL_FLOAT_MAT,4)

SET_UNIFORM_MATRIX(dmat,GL_DOUBLE_MAT,2)
SET_UNIFORM_MATRIX(dmat,GL_DOUBLE_MAT,3)
SET_UNIFORM_MATRIX(dmat,GL_DOUBLE_MAT,4)

#undef SET_UNIFORM_MATRIX
