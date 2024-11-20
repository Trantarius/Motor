#pragma once
#include <mutex>
#include <thread>
#include <semaphore>
#include <list>
#include <condition_variable>
#include <functional>
#include <set>
#include "Object.hpp"

enum TaskStatus{
	TASK_DONE, TASK_EXPIRED, TASK_DEFER
};

typedef std::function<TaskStatus(void)> Task;

class TaskPool{
protected:

	mutable std::mutex mtx;
	std::list<Task> tasks;
	std::counting_semaphore<> task_count_sema {0};
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


enum CallbackResponse{
	CALLBACK_DONE, CALLBACK_EXPIRED, CALLBACK_DEFER
};

template<typename...Args>
class Callback{
	std::shared_ptr<Object*> obj;
	CallbackResponse (*fptr)(Object*,Args...) = nullptr;
	template<typename T,T t>
	friend class _mk_callback;
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
	CallbackResponse operator()(Args...args) const{
		if(obj && *obj)
			return fptr(*obj,std::forward<Args>(args)...);
		return CALLBACK_EXPIRED;
	}
};

template<typename T, T t>
struct _mk_callback;

template<typename Class, typename...Args, typename Ret, Ret(Class::*FPTR)(Args...)> requires std::derived_from<Class,Object>
struct _mk_callback<Ret(Class::*)(Args...),FPTR>{
	static Callback<Args...> make(const Object& obj){
		Callback<Args...> ret;
		ret.obj = obj.self_ptr;
		if constexpr(std::same_as<Ret,CallbackResponse>){
			static constexpr CallbackResponse (*use_ret_func)(Object*,Args...) = [](Object* ptr, Args...args){
				(dynamic_cast<Class*>(ptr)->*FPTR)(std::forward<Args>(args)...);
				return CALLBACK_DONE;
			};
			ret.fptr = use_ret_func;
		}
		else{
			static constexpr CallbackResponse (*ign_ret_func)(Object*,Args...) = [](Object* ptr, Args...args){
				(dynamic_cast<Class*>(ptr)->*FPTR)(std::forward<Args>(args)...);
				return CALLBACK_DONE;
			};
			ret.fptr = ign_ret_func;
		}
		return ret;
	}
};

#define CALLBACK(OBJECT,METHOD) _mk_callback<decltype(&METHOD),&METHOD>::make(OBJECT)

template<typename...Args>
class CallbackList{
	mutable std::mutex mtx;
	std::set<Callback<Args...>> list;
public:

	void add(const Callback<Args...>& cb){
		std::scoped_lock lock(mtx);
		list.insert(cb);
	}

	void remove(const Callback<Args...>& cb){
		std::scoped_lock lock(mtx);
		list.erase(cb);
	}

	Task makeTask(Callback<Args...> cb, const Args&...args){
		return [this,cb,args...](){
			CallbackResponse resp = cb(std::move(args)...);
			if(resp==CALLBACK_EXPIRED){
				this->remove(cb);
				return TASK_EXPIRED;
			}
			if(resp==CALLBACK_DEFER){
				return TASK_DEFER;
			}
			return TASK_DONE;
		};
	}

	void dumpInto(TaskPool& pool,const Args&...args){
		std::list<Task> tasks;
		{
			std::scoped_lock lock(mtx);
			for(const Callback<Args...>& cb : list){
				tasks.push_back(makeTask(cb,args...));
			}
		}
		pool.add_tasks(std::move(tasks));
	}

	CallbackList()=default;
	CallbackList(CallbackList&& b):list(std::move(b.list)){}
};
