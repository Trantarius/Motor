#include "Texture.hpp"
#include "main.hpp"
#include <cassert>
#include <stdexcept>
#include "util/stb_image.h"

struct TextureUnitCounter{
  int max_units;
  bool* occupied;

  TextureUnitCounter(){
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,&max_units);
    occupied=new bool[max_units];
    for(int n=0;n<max_units;n++){
      occupied[n]=false;
    }
  }

  int takeUnit(){
    for(int n=0;n<max_units;n++){
      if(!occupied[n]){
        occupied[n]=true;
        return n;
      }
    }
    throw std::runtime_error("Ran out of texture units");
  }

  void releaseUnit(int unit){
    occupied[unit]=false;
  }

  ~TextureUnitCounter(){
    delete [] occupied;
  }
};

TextureUnitCounter* unitcounter=nullptr;

void Texture::dispose(){
  assert(info!=nullptr);
  glDeleteTextures(1,&info->texture);
  assert(unitcounter!=nullptr);
  unitcounter->releaseUnit(info->unit);
  delete info;
  info==nullptr;
}

Texture::~Texture(){
  info->refcount--;
  if(info->refcount==0){
    dispose();
  }
}

void Texture::operator=(const Texture& md){
  if(info!=nullptr){
    info->refcount--;
    if(info->refcount==0){
      dispose();
    }
  }

  info=md.info;
  if(info!=nullptr){
    info->refcount++;
  }
}

bool Texture::isNull() const {
  return info==nullptr;
}

template<NumberType T>
Texture::Texture(ivec3 size, int channel_count, T* data){

  info=new Info();
  info->refcount+=1;

  assert(channel_count>0 && channel_count<=4);
  assert(size.x>0 && size.y>=0 && size.z>=0);
  if(size.y==0){assert(size.z==0);}

  if(size.y==0 && size.z==0){
    glCreateTextures(GL_TEXTURE_1D,1,&(info->texture));
    info->type=GL_TEXTURE_1D;
  }else if(size.z==0){
    glCreateTextures(GL_TEXTURE_2D,1,&(info->texture));
    info->type=GL_TEXTURE_2D;
  }else{
    glCreateTextures(GL_TEXTURE_3D,1,&(info->texture));
    info->type=GL_TEXTURE_3D;
  }

  if(unitcounter==nullptr){
    unitcounter=new TextureUnitCounter();
  }
  info->unit=unitcounter->takeUnit();
  glActiveTexture(GL_TEXTURE0+info->unit);
  glBindTexture(info->type,info->texture);

  setFilter(LINEAR);
  setWrap(REPEAT);

  glBindTexture(info->type,info->texture);

  static constexpr glEnum formats[4]{GL_RED,GL_RG,GL_RGB,GL_RGBA};
  glEnum format=formats[channel_count-1];

  if(info->type==GL_TEXTURE_1D){
    glTexImage1D(info->type,0,format,size.x,0,format,gl_type<T>::type,data);
  }
  else if(info->type==GL_TEXTURE_2D){
    glTexImage2D(info->type,0,format,size.x,size.y,0,format,gl_type<T>::type,data);
  }
  else if(info->type==GL_TEXTURE_3D){
    glTexImage3D(info->type,0,format,size.x,size.y,size.z,0,format,gl_type<T>::type,data);
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
  info->filter=filter;
  glActiveTexture(GL_TEXTURE0+info->unit);
  glBindTexture(info->type,info->texture);
  if(filter==GL_LINEAR){
    if(info->mipmapped){
      glTexParameteri(info->type,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    }else{
      glTexParameteri(info->type,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    }
  }else{
    if(info->mipmapped){
      glTexParameteri(info->type,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
    }else{
      glTexParameteri(info->type,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    }
  }
  glTexParameteri(info->type,GL_TEXTURE_MAG_FILTER,filter);
}
Texture::Filter Texture::getFilter() const{
  return info->filter;
}
void Texture::setWrap(Wrap wrap){
  assert(!isNull());
  info->wrap=wrap;
  glActiveTexture(GL_TEXTURE0+info->unit);
  glBindTexture(info->type,info->texture);

  switch(info->type){
    case GL_TEXTURE_3D:
      glTexParameteri(info->type,GL_TEXTURE_WRAP_R,wrap);
    case GL_TEXTURE_2D:
      glTexParameteri(info->type,GL_TEXTURE_WRAP_T,wrap);
    case GL_TEXTURE_1D:
      glTexParameteri(info->type,GL_TEXTURE_WRAP_S,wrap);
  }
}
Texture::Wrap Texture::getWrap() const{
  return info->wrap;
}
void Texture::enableMipmaps(){
  assert(!isNull());
  info->mipmapped=true;
  glGenerateTextureMipmap(info->texture);
  setFilter(info->filter);
}
bool Texture::getMipmapped() const{
  return info->mipmapped;
}

Texture Texture::readPNG(string path){

  int width=0,height=0,components=0;
  ushort * data=stbi_load_16(path.c_str(),&width,&height,&components,0);
  assert(data!=nullptr);
  return Texture(ivec3(width,height,0),components,data);
}

glEnum Texture::getType() const{
  assert(info!=nullptr);
  return info->type;
}

int Texture::getUnit() const{
  assert(info!=nullptr);
  return info->unit;
}
