#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int _dummy=[](){
  stbi_set_flip_vertically_on_load(true);
  return 0;
}();
