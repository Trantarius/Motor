#pragma once
#include <string>
#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct glEnum{
	uint val=0;
	constexpr glEnum(){}
	constexpr glEnum(uint e):val(e){}
	operator uint() const {return val;}
	void operator = (uint e){val=e;}
	uint* operator &(){
		return &val;
	}
};

std::string glEnumName(long em);

inline std::string tostr(const glEnum& gle){
	return glEnumName(gle.val);
}

struct OpenGLError : public std::runtime_error{
	glEnum err;
	OpenGLError(std::string what,glEnum err):std::runtime_error(what),err(err){}
};

#define checkGLError() {\
GLenum err=glGetError();\
if(err!=GL_NO_ERROR){\
	throw OpenGLError(std::string("GL error: ")+glEnumName(err)+" " __FILE__ ": " +tostr(__LINE__),err);\
}\
}

namespace gl{
	struct Texture{
		GLuint id;
		Texture(){
			glGenTextures(1,&id);
		}
		~Texture(){
			glDeleteTextures(1,&id);
		}
		operator GLuint() const{return id;}
	};
	struct Framebuffer{
		GLuint id;
		Framebuffer(){
			glGenFramebuffers(1,&id);
		}
		~Framebuffer(){
			glDeleteFramebuffers(1,&id);
		}
		operator GLuint() const{return id;}
	};
	struct Renderbuffer{
		GLuint id;
		Renderbuffer(){
			glGenRenderbuffers(1,&id);
		}
		~Renderbuffer(){
			glDeleteRenderbuffers(1,&id);
		}
		operator GLuint() const{return id;}
	};
};
