#pragma once
#include <cinttypes>
#include <cstdlib>

constexpr uint64_t const_hash(const char* ptr){
  uint64_t ret=0;
  uint8_t off=0;
  uint64_t idx=0;
  do{
    ret ^= ((uint64_t)ptr[idx])<<off;
    off = (off+13)%57;
  }while(ptr[++idx]!=0);
  return ret;
}

class ID{
  uint64_t id=0;
public:

  constexpr ID(){}
  constexpr ID(uint64_t id):id(id){}
  constexpr ID(const char* str):id(const_hash(str)){}

  constexpr operator uint64_t() const {return id;}
  explicit constexpr operator int64_t() const {return id;}

  explicit constexpr operator uint32_t() const {return id ^ id>>32;}
  explicit constexpr operator int32_t() const {return id ^ id>>32;}

  explicit constexpr operator uint16_t() const {return operator uint32_t() ^ operator uint32_t() >>16;}
  explicit constexpr operator int16_t() const {return operator uint32_t() ^ operator uint32_t() >>16;}

  explicit constexpr operator uint8_t() const {return operator uint16_t() ^ operator uint16_t() >>8;}
  explicit constexpr operator int8_t() const {return operator uint16_t() ^ operator uint16_t() >>8;}

  struct Hash{
    constexpr uint64_t operator ()(const ID& id) const {
      return id;
    }
  };

#define COMP_OP(OP)\
  constexpr bool operator OP (const ID& b) const {\
    return id OP b.id;\
  }

  COMP_OP(==)
  COMP_OP(!=)
  COMP_OP(>)
  COMP_OP(<)
  COMP_OP(>=)
  COMP_OP(<=)

#undef COMP_OP

};

consteval ID operator ""_id (const char* ptr,size_t size){
  return ID(ptr);
}



class IndexDistributor{
  int* stack;
  int top;
public:
  const int max;

  IndexDistributor(int max):max(max){
    top=max-1;
    stack=new int[max];
    for(int n=0;n<max;n++){
      stack[n]=top-n;
    }
  }

  int takeIndex(){
    if(top<0){
      return -1;
    }
    return stack[top--];
  }

  void releaseIndex(int unit){
    stack[++top]=unit;
  }

  ~IndexDistributor(){
    delete [] stack;
  }
};
