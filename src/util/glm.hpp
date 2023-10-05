#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "strings.hpp"
#include "bits.hpp"

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

inline uint64_t hash(vec2 v){
  return float_bits(v.x)<<32 ^ float_bits(v.y);
}
inline uint64_t hash(vec3 v){
  return float_bits(v.x)<<32 ^ float_bits(v.y)<<16 ^ float_bits(v.z);
}
inline uint64_t hash(vec4 v){
  return float_bits(v.x)<<32 ^ float_bits(v.y)<<22 ^ float_bits(v.z)<<11 ^ float_bits(v.w);
}

inline uint64_t hash(dvec2 v){
  return bitroll(float_bits(v.x),32) ^ float_bits(v.y);
}
inline uint64_t hash(dvec3 v){
  return bitroll(float_bits(v.x),40) ^ bitroll(float_bits(v.y),20) ^ float_bits(v.z);
}
inline uint64_t hash(dvec4 v){
  return bitroll(float_bits(v.x),48) ^ bitroll(float_bits(v.y),32) ^ bitroll(float_bits(v.z),16) ^ float_bits(v.w);
}

inline uint64_t hash(ivec2 v){
  return ((int64_t)v.x)<<32 ^ ((int64_t)v.y);
}
inline uint64_t hash(ivec3 v){
  return ((int64_t)v.x)<<32 ^ ((int64_t)v.y)<<16 ^ ((int64_t)v.z);
}
inline uint64_t hash(ivec4 v){
  return ((int64_t)v.x)<<32 ^ ((int64_t)v.y)<<22 ^ ((int64_t)v.z)<<11 ^ ((int64_t)v.w);
}

inline uint64_t hash(uvec2 v){
  return ((int64_t)v.x)<<32 ^ ((int64_t)v.y);
}
inline uint64_t hash(uvec3 v){
  return ((int64_t)v.x)<<32 ^ ((int64_t)v.y)<<16 ^ ((int64_t)v.z);
}
inline uint64_t hash(uvec4 v){
  return ((int64_t)v.x)<<32 ^ ((int64_t)v.y)<<22 ^ ((int64_t)v.z)<<11 ^ ((int64_t)v.w);
}

inline bool compare(vec2 a,vec2 b){
  return hash(a)<hash(b);
}
inline bool compare(vec3 a,vec3 b){
  return hash(a)<hash(b);
}
inline bool compare(vec4 a,vec4 b){
  return hash(a)<hash(b);
}

inline bool compare(dvec2 a,dvec2 b){
  return hash(a)<hash(b);
}
inline bool compare(dvec3 a,dvec3 b){
  return hash(a)<hash(b);
}
inline bool compare(dvec4 a,dvec4 b){
  return hash(a)<hash(b);
}

inline bool compare(ivec2 a,ivec2 b){
  return hash(a)<hash(b);
}
inline bool compare(ivec3 a,ivec3 b){
  return hash(a)<hash(b);
}
inline bool compare(ivec4 a,ivec4 b){
  return hash(a)<hash(b);
}

inline bool compare(uvec2 a,uvec2 b){
  return hash(a)<hash(b);
}
inline bool compare(uvec3 a,uvec3 b){
  return hash(a)<hash(b);
}
inline bool compare(uvec4 a,uvec4 b){
  return hash(a)<hash(b);
}
