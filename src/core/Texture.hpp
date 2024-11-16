#pragma once
#include <cstdlib>
#include "util/vec.hpp"
#include "defs/gl_defs.hpp"
#include "util/refcount.hpp"

class Texture{
public:
  enum Filter{NEAREST=GL_NEAREST,LINEAR=GL_LINEAR};
  enum Wrap{REPEAT=GL_REPEAT,MIRROR=GL_MIRRORED_REPEAT,CLAMP=GL_CLAMP_TO_EDGE};

private:
  struct _Data{
    uint refcount=0;

    glEnum texture=0;
    int unit=-1;
    glEnum type=0;

    ivec3 size;

    bool mipmapped=false;
    Filter filter=LINEAR;
    Wrap wrap=REPEAT;
  };

  REF_COUNTER(Texture,_Data)

public:

  template<typename T>
  Texture(ivec3 size, int channel_count, T* texdata);

  void setFilter(Filter filter);
  Filter getFilter() const;
  void setWrap(Wrap wrap);
  Wrap getWrap() const;
  void enableMipmaps();
  bool getMipmapped() const;

  glEnum getType() const;
  int getUnit() const;

  static Texture readPNG(std::string path);
};
