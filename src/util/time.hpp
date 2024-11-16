#pragma once
#include <string>
#include <cstdint>

namespace Time{

	double now();
	uint64_t now_s();
	uint64_t now_ms();
	uint64_t now_us();
	uint64_t now_ns();

	std::string format(double time);
	std::string format_h(double time);
	std::string format_m(double time);
	std::string format_s(double time);
	std::string format_ms(double time);
	std::string format_us(double time);
	std::string format_ns(double time);
	std::string format_hhmmss(double time);
}
