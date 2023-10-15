#pragma once
#include <chrono>
#include <cassert>

const std::chrono::steady_clock::time_point engine_start_time = std::chrono::steady_clock::now();

inline double time(){
  auto now=std::chrono::steady_clock::now();
  std::chrono::duration<double> ret = now - engine_start_time;
  return ret.count();
}

inline uint64_t nanotime(){
  assert(std::chrono::steady_clock::period::den>=1000000000);
  auto now=std::chrono::steady_clock::now();
  std::chrono::steady_clock::duration diff = now - engine_start_time;
  std::chrono::nanoseconds ret = std::chrono::duration_cast<std::chrono::nanoseconds>(diff);
  return ret.count();
}
