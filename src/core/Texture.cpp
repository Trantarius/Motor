#include "Texture.hpp"
#include <cassert>
#include "util/id.hpp"
#include "util/stb_image.h"

//OpenGL spec demands at least 48
IndexDistributor unitcounter(48);

void Texture::onDestroy(){
  glDeleteTextures(1,&data->texture);
  unitcounter.releaseIndex(data->unit);
}

template<typename T>
Texture::Texture(ivec3 size, int channel_count, T* texdata):Texture(){

  assert(channel_count>0 && channel_count<=4);
  assert(size.x>0 && size.y>=0 && size.z>=0);
  if(size.y==0){assert(size.z==0);}

  if(size.y==0 && size.z==0){
    glCreateTextures(GL_TEXTURE_1D,1,&(data->texture));
    data->type=GL_TEXTURE_1D;
  }else if(size.z==0){
    glCreateTextures(GL_TEXTURE_2D,1,&(data->texture));
    data->type=GL_TEXTURE_2D;
  }else{
    glCreateTextures(GL_TEXTURE_3D,1,&(data->texture));
    data->type=GL_TEXTURE_3D;
  }

  data->unit=unitcounter.takeIndex();
  glActiveTexture(GL_TEXTURE0+data->unit);
  glBindTexture(data->type,data->texture);

  setFilter(LINEAR);
  setWrap(REPEAT);

  glBindTexture(data->type,data->texture);

  static constexpr glEnum formats[4]{GL_RED,GL_RG,GL_RGB,GL_RGBA};
  glEnum format=formats[channel_count-1];

  if(data->type==GL_TEXTURE_1D){
    glTexImage1D(data->type,0,format,size.x,0,format,gl_type<T>::type,texdata);
  }
  else if(data->type==GL_TEXTURE_2D){
    glTexImage2D(data->type,0,format,size.x,size.y,0,format,gl_type<T>::type,texdata);
  }
  else if(data->type==GL_TEXTURE_3D){
    glTexImage3D(data->type,0,format,size.x,size.y,size.z,0,format,gl_type<T>::type,texdata);
  }


}

template Texture::Texture(ivec3 size,int channel_count, uint8_t* data);
template Texture::Texture(ivec3 size,int channel_count, int8_t* data);
template Texture::Texture(ivec3 size,int channel_count, uint16_t* data);
template Texture::Texture(ivec3 size,int channel_count, int16_t* data);
template Texture::Texture(ivec3 size,int channel_count, uint32_t* data);
template Texture::Texture(ivec3 size,int channel_count, int32_t* data);

template Texture::Texture(ivec3 size,int channel_count, float* data);
template Texture::Texture(ivec3 size,int channel_count, double* data);

void Texture::setFilter(Filter filter){
  assert(!isNull());
  data->filter=filter;
  glActiveTexture(GL_TEXTURE0+data->unit);
  glBindTexture(data->type,data->texture);
  if(filter==GL_LINEAR){
    if(data->mipmapped){
      glTexParameteri(data->type,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    }else{
      glTexParameteri(data->type,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    }
  }else{
    if(data->mipmapped){
      glTexParameteri(data->type,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
    }else{
      glTexParameteri(data->type,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    }
  }
  glTexParameteri(data->type,GL_TEXTURE_MAG_FILTER,filter);
}
Texture::Filter Texture::getFilter() const{
  return data->filter;
}
void Texture::setWrap(Wrap wrap){
  assert(!isNull());
  data->wrap=wrap;
  glActiveTexture(GL_TEXTURE0+data->unit);
  glBindTexture(data->type,data->texture);

  switch(data->type){
    case GL_TEXTURE_3D:
      glTexParameteri(data->type,GL_TEXTURE_WRAP_R,wrap);
    case GL_TEXTURE_2D:
      glTexParameteri(data->type,GL_TEXTURE_WRAP_T,wrap);
    case GL_TEXTURE_1D:
      glTexParameteri(data->type,GL_TEXTURE_WRAP_S,wrap);
  }
}
Texture::Wrap Texture::getWrap() const{
  return data->wrap;
}
void Texture::enableMipmaps(){
  assert(!isNull());
  data->mipmapped=true;
  glGenerateTextureMipmap(data->texture);
  setFilter(data->filter);
}
bool Texture::getMipmapped() const{
  return data->mipmapped;
}

Texture Texture::readPNG(std::string path){

  int width=0,height=0,components=0;
  ushort * data=stbi_load_16(path.c_str(),&width,&height,&components,0);
  assert(data!=nullptr);
  return Texture(ivec3(width,height,0),components,data);
}

glEnum Texture::getType() const{
  assert(data!=nullptr);
  return data->type;
}

int Texture::getUnit() const{
  assert(data!=nullptr);
  return data->unit;
}
