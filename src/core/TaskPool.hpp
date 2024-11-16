#pragma once
#include <mutex>
#include <thread>
#include <semaphore>
#include <list>
#include <condition_variable>
#include <functional>

enum CallbackResponse{
  CALLBACK_DONE, CALLBACK_EXPIRED, CALLBACK_DEFER
};

template<typename...Args>
class Callback{
  std::weak_ptr<void> obj;
  CallbackResponse (*fptr)(void*,Args...) = nullptr;
public:
  Callback()=default;
  Callback(const Callback&)=default;
  Callback(Callback&&)=default;
  Callback& operator=(const Callback&)=default;
  Callback& operator=(Callback&&)=default;
  std::strong_ordering operator <=> (const Callback& c) const{
		std::strong_ordering obj_o = std::strong_ordering::equal;
		if(obj.owner_before(c.obj))
			obj_o = std::strong_ordering::less;
		else if(c.obj.owner_before(obj))
			obj_o = std::strong_ordering::greater;
		if(obj_o==0)
			return ((size_t)fptr)<=>((size_t)c.fptr);
		return obj_o;
	};
  bool operator==(const Callback&) const = default;
  bool operator!=(const Callback&) const = default;
  bool operator<(const Callback&) const = default;
  bool operator>(const Callback&) const = default;
  bool operator<=(const Callback&) const = default;
  bool operator>=(const Callback&) const = default;
  CallbackResponse operator()(Args...args){
    std::shared_ptr<void> strong = obj.lock();
    if(strong && fptr)
      return fptr(strong.get(),std::forward<Args>(args)...);
    return CALLBACK_EXPIRED;
  }

  template<typename Class, void(Class::*FPTR)(Args...)>
  static Callback from(std::shared_ptr<Class> obj){
    Callback ret;
    ret.obj = obj;
    static constexpr auto func = [](void* ptr, Args...args){
      (static_cast<Class*>(ptr)->*FPTR)(std::forward(args)...);
      return CALLBACK_DONE;
    };
    ret.fptr = func;
    return ret;
  }
};

enum TaskStatus{
	TASK_DONE, TASK_EXPIRED, TASK_DEFER
};

typedef std::function<TaskStatus(void)> Task;

class TaskPool{
protected:

	mutable std::mutex mtx;
	std::list<Task> tasks;
	std::counting_semaphore<> task_count_sema {0};
	std::binary_semaphore empty_wait_sema {1};
	std::condition_variable empty_notifier;
	size_t in_progress = 0;

public:

	TaskPool()=default;
	TaskPool(const TaskPool&)=delete;
	TaskPool(TaskPool&&)=delete;
	TaskPool& operator=(const TaskPool&)=delete;
	TaskPool& operator=(TaskPool&&)=delete;

	virtual ~TaskPool();

	void add_tasks(const std::list<Task>&);
	void add_tasks(std::list<Task>&&);
	void add_task(const Task&);
	void add_task(Task&&);
	size_t queue_size() const;

	virtual void do_one_task();

	// do tasks until there are none left; blocks until completion (even for multithreaded variants)
	virtual void flush();
};

class SingleThreadPool : public TaskPool{

	std::atomic_bool is_destructing = false;
	std::thread thread;

	static void work_loop(SingleThreadPool*);
	using TaskPool::do_one_task;

public:

	SingleThreadPool();
	~SingleThreadPool();
	void flush() override;
};

class MultiThreadPool : public TaskPool{

	bool is_destructing = false;
	std::list<std::thread> threads;

	static void work_loop(MultiThreadPool*);
	using TaskPool::do_one_task;

public:

	MultiThreadPool();
	~MultiThreadPool();
	void flush() override;
};

class TaskCycle : public TaskPool{
protected:
	std::list<Task> offhand;

public:

	TaskCycle()=default;
	TaskCycle(const TaskCycle&)=delete;
	TaskCycle(TaskCycle&&)=delete;
	TaskCycle& operator=(const TaskCycle&)=delete;
	TaskCycle& operator=(TaskCycle&&)=delete;

	void recycle();
	size_t offhand_size() const;
	size_t total_size() const;

	virtual void do_one_task() override;
};

class SingleThreadCycle : public TaskCycle{

	std::atomic_bool is_destructing = false;
	std::thread thread;

	static void work_loop(SingleThreadCycle*);
	using TaskCycle::do_one_task;

public:

	SingleThreadCycle();
	~SingleThreadCycle();
	void flush() override;
};

class MultiThreadCycle : public TaskCycle{

	bool is_destructing = false;
	std::list<std::thread> threads;

	static void work_loop(MultiThreadCycle*);
	using TaskCycle::do_one_task;

public:

	MultiThreadCycle();
	~MultiThreadCycle();
	void flush() override;
};
