#pragma once
#include <glm/glm.hpp>
#include "strings.hpp"

using namespace glm;


template <> inline string tostr(vec2 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+"]";
}
template <> inline string tostr(vec3 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+", "+tostr(v.z)+"]";
}
template <> inline string tostr(vec4 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+", "+tostr(v.z)+", "+tostr(v.w)+"]";
}

template <> inline string tostr(dvec2 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+"]";
}
template <> inline string tostr(dvec3 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+", "+tostr(v.z)+"]";
}
template <> inline string tostr(dvec4 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+", "+tostr(v.z)+", "+tostr(v.w)+"]";
}

template <> inline string tostr(ivec2 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+"]";
}
template <> inline string tostr(ivec3 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+", "+tostr(v.z)+"]";
}
template <> inline string tostr(ivec4 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+", "+tostr(v.z)+", "+tostr(v.w)+"]";
}

template <> inline string tostr(uvec2 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+"]";
}
template <> inline string tostr(uvec3 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+", "+tostr(v.z)+"]";
}
template <> inline string tostr(uvec4 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+", "+tostr(v.z)+", "+tostr(v.w)+"]";
}

template <> inline string tostr(bvec2 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+"]";
}
template <> inline string tostr(bvec3 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+", "+tostr(v.z)+"]";
}
template <> inline string tostr(bvec4 v){
  return string("[")+tostr(v.x)+", "+tostr(v.y)+", "+tostr(v.z)+", "+tostr(v.w)+"]";
}




inline string strWidth(string s){
  s.resize(6,' ');
  return s;
}

template <> inline string tostr(mat2 m){
  m=transpose(m);
  return string("[[")+strWidth(tostr(m[0][0]))+", "+strWidth(tostr(m[0][1]))+"]]\n"+
         string("[[")+strWidth(tostr(m[1][0]))+", "+strWidth(tostr(m[1][1]))+"]]\n";
}
template <> inline string tostr(mat3 m){
  m=transpose(m);
  return string("[[")+strWidth(tostr(m[0][0]))+", "+strWidth(tostr(m[0][1]))+", "+strWidth(tostr(m[0][2]))+"]]\n"+
         string("[[")+strWidth(tostr(m[1][0]))+", "+strWidth(tostr(m[1][1]))+", "+strWidth(tostr(m[1][2]))+"]]\n"+
         string("[[")+strWidth(tostr(m[2][0]))+", "+strWidth(tostr(m[2][1]))+", "+strWidth(tostr(m[2][2]))+"]]\n";
}
template <> inline string tostr(mat4 m){
  m=transpose(m);
  return string("[[")+strWidth(tostr(m[0][0]))+", "+strWidth(tostr(m[0][1]))+", "+strWidth(tostr(m[0][2]))+", "+strWidth(tostr(m[0][3]))+"]]\n"+
         string("[[")+strWidth(tostr(m[1][0]))+", "+strWidth(tostr(m[1][1]))+", "+strWidth(tostr(m[1][2]))+", "+strWidth(tostr(m[1][3]))+"]]\n"+
         string("[[")+strWidth(tostr(m[2][0]))+", "+strWidth(tostr(m[2][1]))+", "+strWidth(tostr(m[2][2]))+", "+strWidth(tostr(m[2][3]))+"]]\n"+
         string("[[")+strWidth(tostr(m[3][0]))+", "+strWidth(tostr(m[3][1]))+", "+strWidth(tostr(m[3][2]))+", "+strWidth(tostr(m[3][3]))+"]]\n";
}

template <> inline string tostr(dmat2 m){
  m=transpose(m);
  return string("[[")+strWidth(tostr(m[0][0]))+", "+strWidth(tostr(m[0][1]))+"]]\n"+
         string("[[")+strWidth(tostr(m[1][0]))+", "+strWidth(tostr(m[1][1]))+"]]\n";
}
template <> inline string tostr(dmat3 m){
  m=transpose(m);
  return string("[[")+strWidth(tostr(m[0][0]))+", "+strWidth(tostr(m[0][1]))+", "+strWidth(tostr(m[0][2]))+"]]\n"+
         string("[[")+strWidth(tostr(m[1][0]))+", "+strWidth(tostr(m[1][1]))+", "+strWidth(tostr(m[1][2]))+"]]\n"+
         string("[[")+strWidth(tostr(m[2][0]))+", "+strWidth(tostr(m[2][1]))+", "+strWidth(tostr(m[2][2]))+"]]\n";
}
template <> inline string tostr(dmat4 m){
  m=transpose(m);
  return string("[[")+strWidth(tostr(m[0][0]))+", "+strWidth(tostr(m[0][1]))+", "+strWidth(tostr(m[0][2]))+", "+strWidth(tostr(m[0][3]))+"]]\n"+
         string("[[")+strWidth(tostr(m[1][0]))+", "+strWidth(tostr(m[1][1]))+", "+strWidth(tostr(m[1][2]))+", "+strWidth(tostr(m[1][3]))+"]]\n"+
         string("[[")+strWidth(tostr(m[2][0]))+", "+strWidth(tostr(m[2][1]))+", "+strWidth(tostr(m[2][2]))+", "+strWidth(tostr(m[2][3]))+"]]\n"+
         string("[[")+strWidth(tostr(m[3][0]))+", "+strWidth(tostr(m[3][1]))+", "+strWidth(tostr(m[3][2]))+", "+strWidth(tostr(m[3][3]))+"]]\n";
}