#pragma once
#include <functional>
#include <type_traits>

template<typename T>
using Func = std::function<T>;

using Call = Func<void(void)>;

#define MemFunc(PTR,FN) std::bind(std::mem_fn(&std::remove_pointer<decltype(PTR)>::type::FN),PTR)
