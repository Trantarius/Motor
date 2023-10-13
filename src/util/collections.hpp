#pragma once
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include "Bloc.hpp"

template<typename L,typename R>
using Pair=std::pair<L,R>;

template<typename T>
using Array = std::vector<T>;

template<typename T>
using List = std::list<T>;

template<typename K,typename V, typename Hash=std::hash<K>, typename Pred=std::equal_to<K>>
using Map = std::unordered_map<K,V,Hash,Pred>;

template<typename K,typename V, typename Hash=std::hash<K>, typename Pred=std::equal_to<K>>
using Set = std::unordered_set<K,V,Hash,Pred>;
