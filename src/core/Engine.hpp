#pragma once
#include <atomic>

class Engine{
	inline static std::atomic_bool is_quitting = false;
public:

	static void init();
	static void main_loop();
	//void terminate();
	static void quit();
};
