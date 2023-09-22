
#pragma once
#include <cstdio>
#include <stdexcept>
#include "Bloc.hpp"
using std::string;

inline Bloc<char> readfile(string path){
  FILE* file = fopen(path.c_str(),"rb");
  if(file==NULL){
    perror(NULL);
    throw std::runtime_error("Cannot open file: "+path);
  }
  fseek(file,0,SEEK_END);
  size_t len=ftell(file);
  rewind(file);

  Bloc<char> ret(len+1);
  size_t count=fread(ret,1,len,file);
  fclose(file);
  if(count!=len){
    ret.destroy();
    perror(NULL);
    throw std::runtime_error("Error reading file: "+path);
  }
  return ret;
}

inline void writefile(string path, Bloc<char> data){
  FILE* file = fopen(path.c_str(),"wb");
  if(file==NULL){
    perror(NULL);
    throw std::runtime_error("Cannot open file: "+path);
  }

  size_t count=fwrite(data,1,data.size,file);
  fclose(file);
  if(count!=data.size){
    perror(NULL);
    throw std::runtime_error("Error writing file: "+path);
  }
}
