#pragma once
#include <atomic>
#include "TaskPool.hpp"

struct Window;

class Engine{
	inline static std::atomic_bool is_quitting = false;
public:

	inline static bool print_fps = false;

	inline static TaskPool main_thread;
	inline static CallbackList<> frame_cycle;
	inline static std::unique_ptr<Window> main_window;

	static void init();
	static void mainLoop();
	// causes mainLoop() to return
	static void quit();
	// destroys everything and exits the program
	static void terminate();
};
