#pragma once
#include <atomic>
#include "TaskPool.hpp"

class Engine{
	inline static std::atomic_bool is_quitting = false;
public:

	inline static bool print_fps = false;
	inline static TaskCycle frame_cycle;

	static void init();
	static void mainLoop();
	// causes mainLoop() to return
	static void quit();
	// destroys everything and exits the program
	static void terminate();
};
