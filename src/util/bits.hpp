#pragma once
#include <cinttypes>

inline uint64_t float_bits(float f){
  return *reinterpret_cast<uint32_t*>(&f);
}

inline uint64_t float_bits(double f){
  return *reinterpret_cast<uint64_t*>(&f);
}

inline uint64_t bitroll(uint64_t a,int8_t b){
  return a<<b ^ a>>(64-b);
}
