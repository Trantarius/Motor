#include "Input.hpp"
#include "Window.hpp"
#include "util/time.hpp"

string Input::keyName(Key key){
  return keyNames.at(key);
}

bool Input::getKey(Key key){
  return keyStates[key];
}

void Input::setCursorMode(int to){
  cursor_mode=to;
  glfwSetInputMode(Window::glfw(),GLFW_CURSOR,to);
}
int Input::getCursorMode(){
  return cursor_mode;
}

void Input::update(){
  double now=time();
  fvec2 mrel=mouse_pos-last_mouse_pos;
  double dt=now-last_update;
  fvec2 mvel=mrel/dt;
  if(len(mvel)>10000){
    mvel=fvec2(0,0);
  }
  mouse_vel=mvel;
  last_update=now;
  last_mouse_pos=mouse_pos;
}

std::function<TaskStatus(void)> generic_listener_task(const Callback<Key,bool>& listener, Key key, bool pressed){
  static constexpr TaskStatus(*func)(Callback<Key,bool>,Key,bool) = [](Callback<Key,bool> callback, Key key, bool pressed){
    CallbackResponse resp = callback(key,pressed);
    if(resp==CALLBACK_EXPIRED){
      Input::removeGenericListener(callback);
    }
    return TASK_DONE;
  };
  return std::bind(func,listener,key,pressed);
}

std::function<TaskStatus(void)> key_listener_task(const Callback<bool>& listener, Key key, bool pressed){
  static constexpr TaskStatus(*func)(Callback<bool>,Key,bool) = [](Callback<bool> callback, Key key,bool pressed){
    CallbackResponse resp = callback(pressed);
    if(resp==CALLBACK_EXPIRED){
      Input::removeKeyListener(key,callback);
    }
    return TASK_DONE;
  };
  return std::bind(func,listener,key,pressed);
}

std::function<TaskStatus(void)> keypress_listener_task(const Callback<>& listener, Key key){
  static constexpr TaskStatus(*func)(Callback<>,Key) = [](Callback<> callback, Key key){
    CallbackResponse resp = callback();
    if(resp==CALLBACK_EXPIRED){
      Input::removeKeypressListener(key,callback);
    }
    return TASK_DONE;
  };
  return std::bind(func,listener,key);
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
  if(action==GLFW_PRESS){
    bool is_press = action==GLFW_PRESS;
    keyStates[(Key)key]=is_press;
    std::list<std::function<TaskStatus(void)>> tasks;
    for(const Callback<Key,bool>& listener : generic_listeners){
      tasks.push_back(generic_listener_task(listener, (Key)key, is_press));
    }
    for(const Callback<bool>& listener : key_listeners[(Key)key]){
      tasks.push_back(key_listener_task(listener, (Key)key, is_press));
    }
    if(is_press){
      for(const Callback<>& listener : keypress_listeners[(Key)key]){
        tasks.push_back(keypress_listener_task(listener, (Key)key));
      }
    }
    input_event_pool.add_tasks(std::move(tasks));
    input_event_pool.flush();
  }
}

void Input::cursorPosCallback(GLFWwindow* window, double xpos, double ypos){
  mouse_pos = fvec2(xpos,ypos);
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
  keyStates[(Key)button]=(action==GLFW_PRESS);
}


void Input::init(){
  glfwSetKeyCallback(Window::glfw(),Input::keyCallback);
  glfwSetCursorPosCallback(Window::glfw(),Input::cursorPosCallback);

  for(auto pr : keyNames){
    keyStates.emplace(pr.first,false);
    key_listeners.emplace(pr.first,std::set<Callback<bool>>());
    keypress_listeners.emplace(pr.first,std::set<Callback<>>());
  }
}

void Input::addGenericListener(const Callback<Key,bool>& listener){
  generic_listeners.insert(listener);
}

void Input::removeGenericListener(const Callback<Key,bool>& listener){
  generic_listeners.erase(listener);
}

void Input::addKeyListener(Key key,const Callback<bool>& listener){
  key_listeners[key].insert(listener);
}
void Input::removeKeyListener(Key key,const Callback<bool>& listener){
  key_listeners[key].erase(listener);
}

void Input::addKeypressListener(Key key,const Callback<>& listener){
  keypress_listeners[key].insert(listener);
}
void Input::removeKeypressListener(Key key,const Callback<>& listener){
  keypress_listeners[key].erase(listener);
}
