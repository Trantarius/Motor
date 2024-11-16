#pragma once
#include <iostream>
#include <string>
#include "strings.hpp"
using std::string;

#ifndef CONCEPT_PRINTABLE
#define CONCEPT_PRINTABLE
template<typename T>
concept Printable = requires(T a){
	{tostr(a)}->std::same_as<string>;
};
#endif

template<Printable...Ts>
void print(Ts...args){
	(std::cout<<...<<tostr(args))<<std::endl;
}

inline bool setgetPrintColor(int set=-1){
	static bool enableColor=false;
	if(set==0){
		enableColor=false;
	}
	if(set==1){
		enableColor=true;
	}
	return enableColor;
}

template<Printable...Ts>
void printerr(Ts...args){
	if(setgetPrintColor()){
		std::cerr<<"\033[91m";//set color to red
	}
	(std::cerr<<...<<tostr(args));
	if(setgetPrintColor()){
		std::cerr<<"\033[0m";//reset
	}
	std::cerr<<std::endl;
}

template<Printable...Ts, Printable T>
void print_(Ts...args, T last){
	(std::cout<<...<<(tostr(args)+" "))<<(tostr(last))<<std::endl;
}

template<Printable T>
void _printsep(string& sep,T arg){
	std::cout<<tostr(arg)<<std::endl;
}

template<Printable T,Printable T2,Printable...Ts>
void _printsep(string& sep,T arg,T2 arg2,Ts...args){
	std::cout<<tostr(arg)<<sep;
	_printsep(sep,arg2,args...);
}

template<Printable...Ts>
void printsep(string sep,Ts...args){
	_printsep(sep,args...);
}

template<Printable...Ts>
void printw(size_t width,Ts...args){
	static auto ensurewidth=[](size_t width,string s)->string{
		s.resize(width,' ');
		return s;
	};
	(std::cout<<...<<ensurewidth(width,tostr(args)))<<std::endl;
}

inline void print_loadbar(double completion){
	string out="\r[";
	int total_length=64;
	int filled=total_length*completion;
	for(int n=0;n<filled;n++){
		out+='#';
	}
	for(int n=0;n<total_length-filled;n++){
		out+=' ';
	}
	out+="]\r";
	std::cout<<out;
	std::cout.flush();
}

