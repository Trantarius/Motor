#pragma once
#include <concepts>
#include <cstdint>
#include <string>
#include <iostream>
#include <cinttypes>
#include <cstdlib>
#include "meta.hpp"

typedef unsigned char uchar;

using std::string;
typedef const char* const_cstr_t;
typedef char* cstr_t;


inline string tostr(string& a){
  return a;
}
inline string tostr(const string& a){
  return a;
}

//casts to int first to force the char to print as a number
inline string tostr(char a){
  return std::to_string((int)a);
}

inline string tostr(bool b){
  return b?"true":"false";
}

#define STD(type) \
inline string tostr(type a){ \
  return std::to_string(a); \
}

STD(u_char)
STD(int)  STD(uint)
STD(short)  STD(ushort)
STD(long)   STD(ulong)
STD(float)  STD(double)

#undef STD





inline string size_format(size_t size){
  if(size>1000L*1000L*1000l*1000L*10L){
    return std::to_string(size/(1000L*1000L*1000l*1000L))+" TB";
  }
  if(size>1000L*1000L*1000l*10L){
    return std::to_string(size/(1000L*1000L*1000l))+" GB";
  }
  if(size>1000L*1000L*10L){
    return std::to_string(size/(1000L*1000L))+" MB";
  }
  if(size>1000L*10L){
    return std::to_string(size/(1000L))+" KB";
  }
  return std::to_string(size)+" B";
}



inline string hexstr(uint8_t n){
  char cstr[3];
  cstr[2]='\0';
  static const char hexdigits[]{'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
  cstr[0]=hexdigits[n>>4];
  cstr[1]=hexdigits[n&0xf];
  return string(cstr);
}

inline string hexstr(uint16_t n){
  return hexstr((uint8_t)(n>>8))+hexstr((uint8_t)(n&0xff));
}

inline string hexstr(uint32_t n){
  return hexstr((uint16_t)(n>>16))+hexstr((uint16_t)(n&0xffff));
}

inline string hexstr(uint64_t n){
  return hexstr((uint32_t)(n>>32))+hexstr((uint32_t)(n&0xffffffff));
}

inline string hexstr(void* p){
  return hexstr((uint64_t)p);
}



inline string tostr(void* a){
  return hexstr((uint64_t)a);
};

inline string tostr(const char* a){
  return string(a);
}

inline string tostr(char* a){
  return string(a);
}


template<typename T>
concept Printable = requires(T a){
  {tostr(a)}->std::same_as<string>;
};

template<Printable...Ts>
void print(Ts...args){
  (std::cout<<...<<tostr(args))<<std::endl;
}

template<Printable...Ts>
void printerr(Ts...args){
  (std::cerr<<...<<tostr(args))<<std::endl;
}

template<Printable...Ts>
void print_(Ts...args){
  (std::cout<<...<<(tostr(args)+" "))<<std::endl;
}

template<Printable T>
void _printsep(string& sep,T arg){
  std::cout<<tostr(arg)<<std::endl;
}

template<Printable T,Printable T2,Printable...Ts>
void _printsep(string& sep,T arg,T2 arg2,Ts...args){
  std::cout<<tostr(arg)<<sep;
  _printsep(sep,arg2,args...);
}

template<Printable...Ts>
void printsep(string sep,Ts...args){
  _printsep(sep,args...);
}

template<Printable...Ts>
void printw(size_t width,Ts...args){
  static auto ensurewidth=[](size_t width,string s)->string{
    s.resize(width,' ');
    return s;
  };
  (std::cout<<...<<ensurewidth(width,tostr(args)))<<std::endl;
}

inline void print_loadbar(double completion){
  string out="\r[";
  int total_length=64;
  int filled=total_length*completion;
  for(int n=0;n<filled;n++){
    out+='#';
  }
  for(int n=0;n<total_length-filled;n++){
    out+=' ';
  }
  out+="]\r";
  std::cout<<out;
  std::cout.flush();
}


#define _MEM_TO_STRING(NAME) \
if constexpr (std::is_same<decltype(s.NAME),string>::value || std::is_same<decltype(s.NAME),const char*>::value){\
  ret+= "  " #NAME ":\t\"" + tostr(s.NAME) + "\"\n";\
}else{\
  ret+= "  " #NAME ":\t" + tostr(s.NAME) + "\n";\
}

//creates a tostr() for a struct, used in form STRUCT_TO_STRING(myClass,mem1,mem2,mem3)
#define STRUCT_TO_STRING(STRUCT,...)\
inline string tostr(const STRUCT& s){\
  string ret = #STRUCT "{\n";\
  FOR_EACH(_MEM_TO_STRING,__VA_ARGS__)\
  ret += "}";\
  return ret;\
}
