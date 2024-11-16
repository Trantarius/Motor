#include "time.hpp"
#include <chrono>
#include <cmath>

const std::chrono::steady_clock::time_point program_start_time = std::chrono::steady_clock::now();

double Time::now(){
	auto now=std::chrono::steady_clock::now();
	std::chrono::duration<double> ret = now - program_start_time;
	return ret.count();
}

uint64_t Time::now_s(){
	auto now=std::chrono::steady_clock::now();
	std::chrono::steady_clock::duration diff = now - program_start_time;
	std::chrono::nanoseconds ret = std::chrono::duration_cast<std::chrono::seconds>(diff);
	return ret.count();
}

uint64_t Time::now_ms(){
	auto now=std::chrono::steady_clock::now();
	std::chrono::steady_clock::duration diff = now - program_start_time;
	std::chrono::nanoseconds ret = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
	return ret.count();
}

uint64_t Time::now_us(){
	auto now=std::chrono::steady_clock::now();
	std::chrono::steady_clock::duration diff = now - program_start_time;
	std::chrono::nanoseconds ret = std::chrono::duration_cast<std::chrono::microseconds>(diff);
	return ret.count();
}

uint64_t Time::now_ns(){
	auto now=std::chrono::steady_clock::now();
	std::chrono::steady_clock::duration diff = now - program_start_time;
	std::chrono::nanoseconds ret = std::chrono::duration_cast<std::chrono::nanoseconds>(diff);
	return ret.count();
}

std::string Time::format(double t){
	if(fabs(t)>3600)
		return Time::format_h(t);
	if(fabs(t)>60)
		return Time::format_m(t);
	if(fabs(t)>1)
		return Time::format_s(t);
	if(fabs(t)>0.001)
		return Time::format_ms(t);
	if(fabs(t)>0.000001)
		return Time::format_us(t);
	return Time::format_ns(t);
}

std::string Time::format_h(double t){
	return std::to_string((int)(t/3600))+"h";
}

std::string Time::format_m(double t){
	return std::to_string((int)(t/60))+"m";
}

std::string Time::format_s(double t){
	return std::to_string((int)(t))+"s";
}

std::string Time::format_ms(double t){
	return std::to_string((int)(t*1000))+"ms";
}

std::string Time::format_us(double t){
	return std::to_string((int)(t*1000000))+"us";
}

std::string Time::format_ns(double t){
	return std::to_string((int)(t*1000000000))+"ns";
}

std::string Time::format_hhmmss(double t){
	bool negative = t<0;
	int hours = t/3600;
	t = fmod(t,3600);
	int minutes = t/60;
	t = fmod(t,60);
	int seconds = t;
	return (negative?"-":"")+std::format("{:02}:{:02}:{:02}",hours,minutes,seconds);
}
