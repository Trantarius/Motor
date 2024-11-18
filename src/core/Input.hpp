#pragma once
#include "defs/keys.hpp"
#include "util/vec.hpp"
#include "TaskPool.hpp"
#include "defs/gl_defs.hpp"  // IWYU pragma: keep
#include <map>

class Input{
	inline static int cursor_mode=GLFW_CURSOR_NORMAL;
	inline static dvec2 mouse_pos{};
	inline static std::map<Key,bool> keyStates;

	inline static TaskPool input_event_pool;

	static void handleEvent(int key,int action);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void scrollCallback(GLFWwindow* window, double x, double y);
public:
	static std::string keyName(Key key);
	static bool isKeyPressed(Key key);

	static void setCursorMode(int to);
	static int getCursorMode();

	static dvec2 getMousePos(){return mouse_pos;}

	inline static CallbackList<Key,bool> generic_listeners;
	inline static std::map<Key,CallbackList<bool>> key_listeners;
	inline static std::map<Key,CallbackList<>> keypress_listeners;

	inline static bool print_events = false;

	static void init();
};
