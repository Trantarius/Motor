#pragma once
#include <string>
#include <cstdint>
#include "meta.hpp"

inline std::string tostr(std::string& a){
	return a;
}
inline std::string tostr(const std::string& a){
	return a;
}

inline std::string tostr(const char* a){
	return std::string(a);
}

inline std::string tostr(char* a){
	return std::string(a);
}

//casts to int first to force the char to print as a number
inline std::string tostr(char a){
	return std::to_string((int)a);
}

inline std::string tostr(bool b){
	return b?"true":"false";
}

#define STD(type) \
inline std::string tostr(type a){ \
	return std::to_string(a); \
}

STD(u_char)
STD(int)  STD(uint)
STD(short)  STD(ushort)
STD(long)   STD(ulong)
STD(float)  STD(double)

#undef STD



inline std::string size_format(size_t size){
	if(size>1000L*1000L*1000l*1000L*10L){
		return std::to_string(size/(1000L*1000L*1000l*1000L))+" TB";
	}
	if(size>1000L*1000L*1000l*10L){
		return std::to_string(size/(1000L*1000L*1000l))+" GB";
	}
	if(size>1000L*1000L*10L){
		return std::to_string(size/(1000L*1000L))+" MB";
	}
	if(size>1000L*10L){
		return std::to_string(size/(1000L))+" KB";
	}
	return std::to_string(size)+" B";
}



inline std::string hexstr(uint8_t n){
	char cstr[3];
	cstr[2]='\0';
	static const char hexdigits[]{'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	cstr[0]=hexdigits[n>>4];
	cstr[1]=hexdigits[n&0xf];
	return std::string(cstr);
}

inline std::string hexstr(uint16_t n){
	return hexstr((uint8_t)(n>>8))+hexstr((uint8_t)(n&0xff));
}

inline std::string hexstr(uint32_t n){
	return hexstr((uint16_t)(n>>16))+hexstr((uint16_t)(n&0xffff));
}

inline std::string hexstr(uint64_t n){
	return hexstr((uint32_t)(n>>32))+hexstr((uint32_t)(n&0xffffffff));
}

inline std::string hexstr(void* p){
	return hexstr((uint64_t)p);
}



inline std::string tostr(void* a){
	return hexstr((uint64_t)a);
};



template<typename T>
concept StructToStringAble = requires(T a,std::string b){
	{structToString(a,b)}->std::same_as<std::string>;
};

template<typename T> requires (!StructToStringAble<T>) && (!std::convertible_to<T,std::string>)
std::string _structToStringEntry(std::string name,const T& val,std::string tab){
	return tab + name + ":  " + tostr(val) + "\n";
}

template<StructToStringAble T>
std::string _structToStringEntry(std::string name, const T& val,std::string tab){
	return tab + name + ":  " + structToString(val,tab) + "\n";
}

inline std::string _structToStringEntry(std::string name,std::string val,std::string tab){
	return tab + name + ":  \"" + val + "\"\n";
}

#define _MEM_TO_STRING(NAME) \
ret += _structToStringEntry(#NAME,s.NAME,mytab);

//creates a tostr() for a struct, used in form STRUCT_TO_STRING(myClass,mem1,mem2,mem3)
#define STRUCT_TO_STRING(STRUCT,...) \
inline std::string structToString(const STRUCT& s, std::string tab){\
	std::string mytab = tab + "  ";\
	std::string ret = #STRUCT "{\n";\
	\
	FOR_EACH(_MEM_TO_STRING,__VA_ARGS__)\
	\
	ret += tab + "}";\
	return ret;\
}\
\
inline std::string tostr(const STRUCT& s){\
	return structToString(s,"");\
}
