#pragma once
#include <string>
#include <stdexcept>
#include "util/math.hpp"
#include "util/id.hpp"
#include "util/collections.hpp"
#include "util/gl_enum_names.hpp"
#include "Texture.hpp"
#include "util/strings.hpp"
#include "util/mem.hpp"


struct UniformBlockMember{
  ID id;
  string name;
  int index;
  int size;//for arrays
  glEnum type;

  int offset;
  int array_stride;
  int matrix_stride;
};

struct UniformBlock{
  ID id;
  string name;
  int index;
  int data_size;

  Map<ID,UniformBlockMember,ID::Hash> members;
};

struct UniformInfo{
  ID id;
  string name;
  int index;
  int location;
  int size;//for arrays
  glEnum type;
};

STRUCT_TO_STRING(UniformInfo,id,name,index,location,size,type)
STRUCT_TO_STRING(UniformBlock,id,name,index,data_size)
STRUCT_TO_STRING(UniformBlockMember,id,name,index,size,type,offset,array_stride,matrix_stride);

class UniformBuffer;

class Shader{

  struct _Data{
    uint refcount=0;
    uint gl_program=0;
    Map<ID,UniformInfo,ID::Hash> uniforms;
    Map<ID,UniformBlock,ID::Hash> uniform_blocks;
  };

  REF_COUNTER(Shader,_Data)

public:

  struct CompileError : public std::runtime_error{
    CompileError(string what):std::runtime_error(what){}
  };

  Shader(string vertex_shader,string fragment_shader);

  void use() const;

  template<typename T>
  void setUniform(ID name,T val);

  template<typename T>
  void setUniformArray(ID name,const Bloc<T> vals);

  void setUniformBlock(ID name,UniformBuffer buffer);

  Array<UniformInfo> getUniforms() const;
  UniformInfo getUniform(ID id) const;
  bool hasUniform(ID id) const;
  Array<UniformBlock> getUniformBlocks() const;
  UniformBlock getUniformBlock(ID id) const;
  bool hasUniformBlock(ID id) const;

  // recursively load all shader files (*.glsl) found at path into named strings for #include directives
  // names will be relative to path
  static void loadAllShaderFiles(string path);
};

template <> void Shader::setUniform(ID name,float val);
template <> void Shader::setUniformArray(ID name,const Bloc<float> val);
template <> void Shader::setUniform(ID name,fvec2 val);
template <> void Shader::setUniformArray(ID name,const Bloc<fvec2> val);
template <> void Shader::setUniform(ID name,fvec3 val);
template <> void Shader::setUniformArray(ID name,const Bloc<fvec3> val);
template <> void Shader::setUniform(ID name,fvec4 val);
template <> void Shader::setUniformArray(ID name,const Bloc<fvec4> val);

template <> void Shader::setUniform(ID name,double val);
template <> void Shader::setUniformArray(ID name,const Bloc<double> val);
template <> void Shader::setUniform(ID name,dvec2 val);
template <> void Shader::setUniformArray(ID name,const Bloc<dvec2> val);
template <> void Shader::setUniform(ID name,dvec3 val);
template <> void Shader::setUniformArray(ID name,const Bloc<dvec3> val);
template <> void Shader::setUniform(ID name,dvec4 val);
template <> void Shader::setUniformArray(ID name,const Bloc<dvec4> val);

template <> void Shader::setUniform(ID name,int val);
template <> void Shader::setUniformArray(ID name,const Bloc<int> val);
template <> void Shader::setUniform(ID name,ivec2 val);
template <> void Shader::setUniformArray(ID name,const Bloc<ivec2> val);
template <> void Shader::setUniform(ID name,ivec3 val);
template <> void Shader::setUniformArray(ID name,const Bloc<ivec3> val);
template <> void Shader::setUniform(ID name,ivec4 val);
template <> void Shader::setUniformArray(ID name,const Bloc<ivec4> val);

template <> void Shader::setUniform(ID name,fmat2 val);
template <> void Shader::setUniformArray(ID name,const Bloc<fmat2> val);
template <> void Shader::setUniform(ID name,fmat3 val);
template <> void Shader::setUniformArray(ID name,const Bloc<fmat3> val);
template <> void Shader::setUniform(ID name,fmat4 val);
template <> void Shader::setUniformArray(ID name,const Bloc<fmat4> val);

template <> void Shader::setUniform(ID name,dmat2 val);
template <> void Shader::setUniformArray(ID name,const Bloc<dmat2> val);
template <> void Shader::setUniform(ID name,dmat3 val);
template <> void Shader::setUniformArray(ID name,const Bloc<dmat3> val);
template <> void Shader::setUniform(ID name,dmat4 val);
template <> void Shader::setUniformArray(ID name,const Bloc<dmat4> val);

template <> void Shader::setUniform(ID name,Texture val);
template <> void Shader::setUniformArray(ID name,const Bloc<Texture> val);



class UniformBuffer{

  static IndexDistributor binding_distributor;

  struct _Data{
    uint refcount=0;
    uint buffer;
    int binding=-1;
    UniformBlock block;
    Bloc<void> update_data;
  };

  REF_COUNTER(UniformBuffer,_Data)

public:

  UniformBuffer(const UniformBlock& block);

  void beginUpdate();
  void endUpdate();

  UniformBlock getUniformBlock() const;

  template<typename T>
  void setUniform(ID name,T val);

  template<typename T>
  void setUniformArray(ID name,const Bloc<T> vals);

  friend void Shader::setUniformBlock(ID id,UniformBuffer buffer);
};

template <> void UniformBuffer::setUniform(ID name,float val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<float> val);
template <> void UniformBuffer::setUniform(ID name,fvec2 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<fvec2> val);
template <> void UniformBuffer::setUniform(ID name,fvec3 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<fvec3> val);
template <> void UniformBuffer::setUniform(ID name,fvec4 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<fvec4> val);

template <> void UniformBuffer::setUniform(ID name,double val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<double> val);
template <> void UniformBuffer::setUniform(ID name,dvec2 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<dvec2> val);
template <> void UniformBuffer::setUniform(ID name,dvec3 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<dvec3> val);
template <> void UniformBuffer::setUniform(ID name,dvec4 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<dvec4> val);

template <> void UniformBuffer::setUniform(ID name,int val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<int> val);
template <> void UniformBuffer::setUniform(ID name,ivec2 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<ivec2> val);
template <> void UniformBuffer::setUniform(ID name,ivec3 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<ivec3> val);
template <> void UniformBuffer::setUniform(ID name,ivec4 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<ivec4> val);

template <> void UniformBuffer::setUniform(ID name,fmat2 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<fmat2> val);
template <> void UniformBuffer::setUniform(ID name,fmat3 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<fmat3> val);
template <> void UniformBuffer::setUniform(ID name,fmat4 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<fmat4> val);

template <> void UniformBuffer::setUniform(ID name,dmat2 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<dmat2> val);
template <> void UniformBuffer::setUniform(ID name,dmat3 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<dmat3> val);
template <> void UniformBuffer::setUniform(ID name,dmat4 val);
template <> void UniformBuffer::setUniformArray(ID name,const Bloc<dmat4> val);
