#include "Input.hpp"
#include "Window.hpp"
#include "util/print.hpp"

std::string Input::keyName(Key key){
	return keyNames.at(key);
}

bool Input::isKeyPressed(Key key){
	return keyStates[key];
}

void Input::setCursorMode(int to){
	cursor_mode=to;
	glfwSetInputMode(Window::glfw(),GLFW_CURSOR,to);
}
int Input::getCursorMode(){
	return cursor_mode;
}

void Input::handleEvent(int key,int action){
	bool is_press = action==GLFW_PRESS;
	Input::keyStates[(Key)key]=is_press;
	generic_listeners.dumpInto(input_event_pool,(Key)key,is_press);
	key_listeners[(Key)key].dumpInto(input_event_pool, is_press);
	keypress_listeners[(Key)key].dumpInto(input_event_pool);
	input_event_pool.flush();
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if(Input::print_events){
		print("Input: keyCallback: key=",key," '",keyName((Key)key),"' scancode=",scancode," action=",action," (",action==GLFW_PRESS?"pressed":"released",") mods=",mods);
	}
	handleEvent(key,action);
}

void Input::cursorPosCallback(GLFWwindow* window, double xpos, double ypos){
	mouse_pos = dvec2(xpos,ypos);
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
	if(Input::print_events){
		print("Input: mouseButtonCallback: button=",button," '",keyName((Key)button),"' action=",action," (",action==GLFW_PRESS?"pressed":"released",") mods=",mods);
	}
	handleEvent(button,action);
}

void Input::scrollCallback(GLFWwindow* window, double x, double y){
	if(Input::print_events){
		print("Input: scrollCallback: x=",x," y=",y);
	}
}

void Input::init(){
	glfwSetKeyCallback(Window::glfw(),Input::keyCallback);
	glfwSetCursorPosCallback(Window::glfw(),Input::cursorPosCallback);
	glfwSetMouseButtonCallback(Window::glfw(),Input::mouseButtonCallback);
	glfwSetScrollCallback(Window::glfw(),Input::scrollCallback);

	for(auto pr : keyNames){
		keyStates.emplace(pr.first,false);
		key_listeners.emplace(pr.first,CallbackList<bool>());
		keypress_listeners.emplace(pr.first,CallbackList<>());
	}

	glfwGetCursorPos(Window::glfw(),&mouse_pos.x,&mouse_pos.y);
}
