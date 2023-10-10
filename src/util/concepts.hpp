#pragma once
#include <type_traits>

template<typename T>
struct AutoPassType{
  typedef T& type;
};

template<typename T> requires std::is_arithmetic<T>::value || std::is_enum<T>::value
struct AutoPassType<T>{
  typedef T type;
};

template <typename T>
struct AutoPassType<T*>{
  typedef T* type;
};

template <typename T>
struct AutoPassType<T&>{
  typedef T& type;
};
