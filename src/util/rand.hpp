#pragma once
#include <random>
#include <chrono>
#include <cassert>
#include "strings.hpp"

struct RandGen{

  static uint64_t getTimeSeed(){
    auto now=std::chrono::high_resolution_clock::now();
    auto dur=now.time_since_epoch();
    return dur.count();
  }

  //produces an integer between 0 and as high as possible.
  virtual uint64_t getInt()=0;
  //returns double in range [0,1) (note: printing and casting to float may round it to 1)
  virtual double getDouble()=0;
  //returns float in range [0,1) (note: printing may round it to 1)
  virtual float getFloat()=0;

  //random with regards to seed and key, but otherwise deterministic
  virtual uint64_t mapInt(uint64_t key) const =0;
  virtual double mapDouble(uint64_t key) const =0;
  virtual float mapFloat(uint64_t key) const =0;

  virtual uint64_t getSeed() const=0;
  virtual void setSeed(uint64_t)=0;

  operator uint64_t(){return getInt();}
  operator int64_t(){return getInt();}
  operator uint32_t(){return getInt();}
  operator int32_t(){return getInt();}
  operator uint16_t(){return getInt();}
  operator int16_t(){return getInt();}
  operator uint8_t(){return getInt();}
  operator int8_t(){return getInt();}
  operator double(){return getDouble();}
  operator float(){return getFloat();}
};

template<typename URBG> requires std::uniform_random_bit_generator<URBG>
class RandGenURBG : public RandGen{
  URBG gen;
  uint64_t rseed;
  uint64_t seed;

  static constexpr uint64_t mask=((uint64_t)1<<60)-1;
  static const uint64_t _min=URBG::min()&mask;
  static const uint64_t _max=URBG::max()&mask;

  static constexpr uint64_t fmask=((uint64_t)1<<48)-1;
  static const uint64_t _fmin=URBG::min()&fmask;
  static const uint64_t _fmax=URBG::max()&fmask;

  static uint64_t _getTimeSeed(){
    auto now=std::chrono::high_resolution_clock::now();
    auto dur=now.time_since_epoch();
    return dur.count();
  }


public:
  //this prevents accidental copying when passing as a parameter
  explicit RandGenURBG(const RandGenURBG& b) = default;

  RandGenURBG():seed(RandGen::getTimeSeed()){
    setSeed(seed);
  }
  RandGenURBG(uint64_t seed):seed(seed){
    setSeed(seed);
  }

  uint64_t getInt() override{
    return gen()-_min;
  }
  double getDouble() override{
    return (double)(gen()&mask-_min)/(double)(_max-_min+0xf0);
  }
  float getFloat() override{
    return (float)(gen()&fmask-_fmin)/(float)(_fmax-_fmin+0x2000000);
  }

  uint64_t mapInt(uint64_t key) const override{
    URBG gen;
    gen.seed(key^rseed);
    return gen()-_min;
  }
  double mapDouble(uint64_t key) const override{
    URBG gen;
    gen.seed(key^rseed);
    return (double)(gen()&mask-_min)/(double)(_max-_min+0xf0);
  }
  float mapFloat(uint64_t key) const override{
    URBG gen;
    gen.seed(key^rseed);
    return (float)(gen()&fmask-_fmin)/(float)(_fmax-_fmin+0x2000000);
  }

  uint64_t getSeed() const override{
    return seed;
  }
  void setSeed(uint64_t to) override{
    seed=to;
    gen.seed(seed);
    rseed=gen();
  }
};

typedef RandGenURBG<std::mt19937_64> RandGenMT;
typedef RandGenURBG<std::random_device> RandGenDevice;
typedef RandGenURBG<std::minstd_rand> RandGenMinStd;
typedef RandGenURBG<std::ranlux48> RandGenLux;
