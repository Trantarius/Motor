#pragma once
#include <string>


std::string glEnumName(long em);

#define checkGLError() {\
GLenum err=glGetError();\
if(err!=GL_NO_ERROR){\
  printerr("GL error: ",glEnumName(err));\
}\
}
