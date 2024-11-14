#pragma once
#include <memory>
#include <mutex>
#include <thread>
#include <semaphore>
#include <list>
#include <condition_variable>
#include <typeindex>

template<typename T>
struct Callable;

template<typename Ret, typename...Args>
struct Callable<Ret(Args...)>{
	virtual Ret operator()(Args...) = 0;
	virtual operator bool() const = 0;
	virtual bool try_call(Ret*, Args...) = 0;
	virtual size_t hash() const = 0;
	virtual std::strong_ordering operator<=>(const Callable<Ret(Args...)>&) const = 0;
	bool operator==(const Callable<Ret(Args...)>&) const = default;
	bool operator!=(const Callable<Ret(Args...)>&) const = default;
	bool operator<(const Callable<Ret(Args...)>&) const = default;
	bool operator>(const Callable<Ret(Args...)>&) const = default;
	bool operator<=(const Callable<Ret(Args...)>&) const = default;
	bool operator>=(const Callable<Ret(Args...)>&) const = default;
};

template<typename...Args>
struct Callable<void(Args...)>{
	virtual void operator()(Args...) = 0;
	virtual operator bool() const = 0;
	virtual bool try_call(Args...) = 0;
	virtual size_t hash() const = 0;
	virtual std::strong_ordering operator<=>(const Callable<void(Args...)>&) const = 0;
	bool operator==(const Callable<void(Args...)>&) const = default;
	bool operator!=(const Callable<void(Args...)>&) const = default;
	bool operator<(const Callable<void(Args...)>&) const = default;
	bool operator>(const Callable<void(Args...)>&) const = default;
	bool operator<=(const Callable<void(Args...)>&) const = default;
	bool operator>=(const Callable<void(Args...)>&) const = default;
};

template<typename T>
struct CallablePtr;

template<typename Ret, typename...Args>
struct CallablePtr<Ret(Args...)> : Callable<Ret(Args...)>{
	std::shared_ptr<Callable<Ret(Args...)>> ptr;

	Ret operator()(Args...args) override {
		return (*ptr)(std::forward(args)...);
	}
	operator bool() const override {
		return ptr && *ptr;
	}
	bool try_call(Ret* out, Args...args) override{
		if(ptr){
			return ptr->try_call(out, std::forward(args)...);
		}else{
			return false;
		}
	}
	size_t hash() const override{
		return ptr ? ptr->hash() : 0;
	}
	std::strong_ordering operator<=>(const Callable<Ret(Args...)>& c) const override{
		if(ptr)
			return (*ptr)<=>c;
		return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
	}

	template<typename T> requires std::derived_from<T,Callable<Ret(Args...)>>
	CallablePtr(const T& t) : ptr(std::make_shared<T>(t)) {}
	template<typename T> requires std::derived_from<T,Callable<Ret(Args...)>>
	CallablePtr(T&& t) : ptr(std::make_shared<T>(std::move(t))) {}

	CallablePtr()=default;
	CallablePtr(const CallablePtr&)=default;
	CallablePtr(CallablePtr&&)=default;
	CallablePtr& operator=(const CallablePtr&)=default;
	CallablePtr& operator=(CallablePtr&&)=default;
};

template<typename...Args>
struct CallablePtr<void(Args...)> : Callable<void(Args...)>{
	std::shared_ptr<Callable<void(Args...)>> ptr;

	void operator()(Args...args) override {
		(*ptr)(std::forward(args)...);
	}
	operator bool() const override {
		return ptr && *ptr;
	}
	bool try_call(Args...args) override{
		if(ptr){
			return ptr->try_call(std::forward(args)...);
		}else{
			return false;
		}
	}
	size_t hash() const override{
		return ptr ? ptr->hash() : 0;
	}
	std::strong_ordering operator<=>(const Callable<void(Args...)>& c) const override{
		if(ptr)
			return (*ptr)<=>c;
		return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
	}

	template<typename T> requires std::derived_from<T,Callable<void(Args...)>>
	CallablePtr(const T& t) : ptr(std::make_shared<T>(t)) {}
	template<typename T> requires std::derived_from<T,Callable<void(Args...)>>
	CallablePtr(T&& t) : ptr(std::make_shared<T>(std::move(t))) {}

	CallablePtr()=default;
	CallablePtr(const CallablePtr&)=default;
	CallablePtr(CallablePtr&&)=default;
	CallablePtr& operator=(const CallablePtr&)=default;
	CallablePtr& operator=(CallablePtr&&)=default;
};

template<typename T>
struct BoundCallable;

template<typename Ret, typename...Args>
struct BoundCallable<Ret(Args...)> : Callable<Ret(void)>{
	std::shared_ptr<Callable<Ret(Args...)>> ptr;
	typedef std::tuple<typename std::decay<Args>::type...> ArgTuple;
	std::shared_ptr<ArgTuple> args;

	Ret operator()() override {
		return std::apply(*ptr, *args);
	}
	operator bool() const override {
		return ptr && *ptr && args;
	}
	bool try_call(Ret* out) override{
		if(ptr && args){
			auto p = ptr;
			return std::apply([p,out](Args...as){ return p->try_call(out,std::forward(as)...); }, *args);
		}else{
			return false;
		}
	}
	size_t hash() const override{
		return (ptr ? ptr->hash() : 0) ^ (args ? std::hash<ArgTuple>()(*args) : 0);
	}
	std::strong_ordering operator<=>(const Callable<Ret(void)>& c) const override{
		if(typeid(c)==typeid(CallablePtr<Ret(void)>)){
			const CallablePtr<Ret(void)>& b = dynamic_cast<const CallablePtr<Ret(void)>&>(c);
			if(b.ptr)
				return (*this)<=>(*b.ptr);
			return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
		}
		if(typeid(*this)!=typeid(c))
			return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
		const BoundCallable<Ret(Args...)>& b = dynamic_cast<const BoundCallable<Ret(Args...)>&>(c);
		if((bool)ptr != (bool)b.ptr)
			return ptr<=>b.ptr;
		if(ptr){
			std::strong_ordering p = (*ptr)<=>(*b.ptr);
			if(p==0){
				if((bool)args != (bool)b.args)
					return args<=>b.args;
				if(args)
					return (*args)<=>(*b.args);
				return std::strong_ordering::equal;
			}
			return p;
		}
		return std::strong_ordering::equal;
	}

	template<typename T> requires std::derived_from<T,Callable<void(Args...)>>
	BoundCallable(const T& t, Args...arg) :
		ptr(std::make_shared<T>(t)), args(std::make_shared<ArgTuple>(std::forward(arg)...)) {}
	template<typename T> requires std::derived_from<T,Callable<void(Args...)>>
	BoundCallable(T&& t, Args...arg) :
	ptr(std::make_shared<T>(std::move(t))), args(std::make_shared<ArgTuple>(std::forward(arg)...)) {}

	BoundCallable()=default;
	BoundCallable(const BoundCallable&)=default;
	BoundCallable(BoundCallable&&)=default;
	BoundCallable& operator=(const BoundCallable&)=default;
	BoundCallable& operator=(BoundCallable&&)=default;
};

template<typename...Args>
struct BoundCallable<void(Args...)> : Callable<void(void)>{
	std::shared_ptr<Callable<void(Args...)>> ptr;
	typedef std::tuple<typename std::decay<Args>::type...> ArgTuple;
	std::shared_ptr<ArgTuple> args;

	void operator()() override {
		return std::apply(*ptr, *args);
	}
	operator bool() const override {
		return ptr && *ptr && args;
	}
	bool try_call() override{
		if(ptr && args){
			auto p = ptr;
			return std::apply([p](Args...as){ return p->try_call(std::forward(as)...); }, *args);
		}else{
			return false;
		}
	}
	size_t hash() const override{
		return (ptr ? ptr->hash() : 0) ^ (args ? std::hash<ArgTuple>()(*args) : 0);
	}
	std::strong_ordering operator<=>(const Callable<void(void)>& c) const override{
		if(typeid(c)==typeid(CallablePtr<void(void)>)){
			const CallablePtr<void(void)>& b = dynamic_cast<const CallablePtr<void(void)>&>(c);
			if(b.ptr)
				return (*this)<=>(*b.ptr);
			return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
		}
		if(typeid(*this)!=typeid(c))
			return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
		const BoundCallable<void(Args...)>& b = dynamic_cast<const BoundCallable<void(Args...)>&>(c);
		if((bool)ptr != (bool)b.ptr)
			return ptr<=>b.ptr;
		if(ptr){
			std::strong_ordering p = (*ptr)<=>(*b.ptr);
			if(p==0){
				if((bool)args != (bool)b.args)
					return args<=>b.args;
				if(args)
					return (*args)<=>(*b.args);
				return std::strong_ordering::equal;
			}
			return p;
		}
		return std::strong_ordering::equal;
	}

	template<typename T> requires std::derived_from<T,Callable<void(Args...)>>
	BoundCallable(const T& t, Args...arg) :
		ptr(std::make_shared<T>(t)), args(std::make_shared<ArgTuple>(std::forward(arg)...)) {}
	template<typename T> requires std::derived_from<T,Callable<void(Args...)>>
	BoundCallable(T&& t, Args...arg) :
	ptr(std::make_shared<T>(std::move(t))), args(std::make_shared<ArgTuple>(std::forward(arg)...)) {}

	BoundCallable()=default;
	BoundCallable(const BoundCallable&)=default;
	BoundCallable(BoundCallable&&)=default;
	BoundCallable& operator=(const BoundCallable&)=default;
	BoundCallable& operator=(BoundCallable&&)=default;
};

template<typename T>
struct Function;

template<typename Ret, typename...Args>
struct Function<Ret(Args...)> : Callable<Ret(Args...)>{
	Ret(*ptr)(Args...) = nullptr;
	Ret operator()(Args...args) override{
		return ptr(std::forward(args)...);
	}
	operator bool() const override{
		return ptr;
	}
	bool try_call(Ret* out, Args...args) override{
		if(ptr){
			*out = ptr(std::forward(args)...);
			return true;
		}else{
			return false;
		}
	}
	size_t hash() const override{
		return ptr;
	}
	std::strong_ordering operator<=>(const Callable<Ret(Args...)>& c) const override{
		if(typeid(c)==typeid(CallablePtr<Ret(Args...)>)){
			const CallablePtr<Ret(Args...)>& b = dynamic_cast<const CallablePtr<Ret(Args...)>&>(c);
			if(b.ptr)
				return (*this)<=>(*b.ptr);
			return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
		}
		if(typeid(*this)!=typeid(c))
			return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
		const Function<Ret(Args...)>& b = dynamic_cast<const Function<Ret(Args...)>&>(c);
		return ptr<=>b.ptr;
	}
	Function()=default;
	Function(const Function&)=default;
	Function(Function&&)=default;
	Function& operator=(const Function&)=default;
	Function& operator=(Function&&)=default;
	Function(Ret(*ptr)(Args...)) : ptr(ptr) {}
};

template<typename...Args>
struct Function<void(Args...)> : Callable<void(Args...)>{
	void(*ptr)(Args...) = nullptr;
	void operator()(Args...args) override{
		ptr(std::forward(args)...);
	}
	operator bool() const override{
		return ptr;
	}
	bool try_call(Args...args) override{
		if(ptr){
			ptr(std::forward(args)...);
			return true;
		}else{
			return false;
		}
	}
	size_t hash() const override{
		return ptr;
	}
	std::strong_ordering operator<=>(const Callable<void(Args...)>& c) const override{
		if(typeid(c)==typeid(CallablePtr<void(Args...)>)){
			const CallablePtr<void(Args...)>& b = dynamic_cast<const CallablePtr<void(Args...)>&>(c);
			if(b.ptr)
				return (*this)<=>(*b.ptr);
			return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
		}
		if(typeid(*this)!=typeid(c))
			return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
		const Function<void(Args...)>& b = dynamic_cast<const Function<void(Args...)>&>(c);
		return ptr<=>b.ptr;
	}
	Function()=default;
	Function(const Function&)=default;
	Function(Function&&)=default;
	Function& operator=(const Function&)=default;
	Function& operator=(Function&&)=default;
	Function(void(*ptr)(Args...)) : ptr(ptr) {}
};

template<typename C, typename T>
struct MemberFunction;

template<typename C, typename Ret, typename...Args>
struct MemberFunction<C,Ret(Args...)> : Callable<Ret(Args...)>{
	std::weak_ptr<C> object;
	Ret(C::*func)(Args...) = nullptr;

	Ret operator()(Args...args) override{
		return object.lock()->*func(std::forward(args)...);
	}
	operator bool() const override{
		return object && func;
	}
	bool try_call(Ret* out, Args...args) override{
		std::shared_ptr<C> obj = object.lock();
		if(obj){
			*out = object->*func(std::forward(args)...);
			return true;
		}else{
			return false;
		}
	}
	size_t hash() const override{
		return std::hash<std::weak_ptr<C>>()(object) ^ func;
	}
	std::strong_ordering operator<=>(const Callable<Ret(Args...)>& c) const override{
		const std::shared_ptr<C> obj = object.lock();
		if(typeid(c)==typeid(CallablePtr<Ret(Args...)>)){
			const CallablePtr<Ret(Args...)>& b = dynamic_cast<const CallablePtr<Ret(Args...)>&>(c);
			if(b.ptr)
				return (*this)<=>(*b.ptr);
			return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
		}
		if(typeid(*this)!=typeid(c))
			return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
		const MemberFunction<C,Ret(Args...)>& b = dynamic_cast<const MemberFunction<C,Ret(Args...)>&>(c);
		const std::shared_ptr<C> bobj = b.object.lock();

		std::strong_ordering f = func<=>b.func;
		if(f!=0)
			return f;
		return obj<=>bobj;
	}

	MemberFunction()=default;
	MemberFunction(const MemberFunction&)=default;
	MemberFunction(MemberFunction&&)=default;
	MemberFunction& operator=(const MemberFunction&)=default;
	MemberFunction& operator=(MemberFunction&&)=default;
	MemberFunction(std::shared_ptr<C> obj, Ret(C::*func)(Args...)) : object(obj), func(func) {}
};

template<typename C,typename...Args>
struct MemberFunction<C,void(Args...)> : Callable<void(Args...)>{
	std::weak_ptr<C> object;
	void(C::*func)(Args...) = nullptr;

	void operator()(Args...args) override{
		object.lock()->*func(std::forward(args)...);
	}
	operator bool() const override{
		return object && func;
	}
	bool try_call(Args...args) override{
		std::shared_ptr<C> obj = object.lock();
		if(obj){
			object->*func(std::forward(args)...);
			return true;
		}else{
			return false;
		}
	}
	size_t hash() const override{
		return std::hash<std::weak_ptr<C>>()(object) ^ func;
	}
	std::strong_ordering operator<=>(const Callable<void(Args...)>& c) const override{
		const std::shared_ptr<C> obj = object.lock();
		if(typeid(c)==typeid(CallablePtr<void(Args...)>)){
			const CallablePtr<void(Args...)>& b = dynamic_cast<const CallablePtr<void(Args...)>&>(c);
			if(b.ptr)
				return (*this)<=>(*b.ptr);
			return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
		}
		if(typeid(*this)!=typeid(c))
			return std::type_index(typeid(*this))<=>std::type_index(typeid(c));
		const MemberFunction<C,void(Args...)>& b = dynamic_cast<const MemberFunction<C,void(Args...)>&>(c);
		const std::shared_ptr<C> bobj = b.object.lock();

		std::strong_ordering f = func<=>b.func;
		if(f!=0)
			return f;
		return obj<=>bobj;
	}

	MemberFunction()=default;
	MemberFunction(const MemberFunction&)=default;
	MemberFunction(MemberFunction&&)=default;
	MemberFunction& operator=(const MemberFunction&)=default;
	MemberFunction& operator=(MemberFunction&&)=default;
	MemberFunction(std::shared_ptr<C> obj, void(C::*func)(Args...)) : object(obj), func(func) {}
};

template<typename T>
struct std::hash<Callable<T>>{
	size_t operator()(const Callable<T>& c) const {
		return c.hash();
	}
};

class TaskLock{
	struct Core{
		static_assert(sizeof(std::thread::id)<=sizeof(size_t));
		mutable std::atomic_size_t owner; // hash of thread id
		mutable size_t depth = 0;
	};
	std::shared_ptr<Core> core = std::make_shared<Core>();
	static bool _is_owned_by_me(const std::shared_ptr<Core>&);
	static bool _is_owned_by_anyone(const std::shared_ptr<Core>&);

	static bool _try_lock(const std::shared_ptr<Core>&);
	static void _unlock(const std::shared_ptr<Core>&);
	friend class Task;
public:

	TaskLock()=default;
	TaskLock(const TaskLock&)=delete;
	TaskLock(TaskLock&&)=delete;
	TaskLock& operator=(const TaskLock&)=delete;
	TaskLock& operator=(TaskLock&&)=delete;

	bool is_owned_by_me() const { return _is_owned_by_me(core); }
	bool is_owned_by_anyone() const { return _is_owned_by_anyone(core); };

	bool try_lock() const { return _try_lock(core); };
	void unlock() const { _unlock(core); };

	~TaskLock();
};

class Task{
	CallablePtr<void(void)> call;
	std::list<std::shared_ptr<TaskLock::Core>> required_locks;
	friend class TaskPool;
public:
	void add_lock(const TaskLock&);
	bool attempt();

	Task()=default;
	Task(const Task&)=default;
	Task(Task&&)=default;
	Task& operator=(const Task&)=default;
	Task& operator=(Task&&)=default;
	Task(const CallablePtr<void(void)>& f) : call(f) {}
};

class TaskPool{
protected:

	mutable std::mutex mtx;
	std::list<Task> tasks;
	std::counting_semaphore<> empty_wait_sema {0};
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
	void add_tasks(const std::list<CallablePtr<void(void)>>&);
	void add_task(const Task& call);
	void add_task(Task&& call);
	size_t queue_size() const;

	// tries to do the next task; if the locks are obtained, the task is completed and then true is
	// returned; otherwise, the locks are released and false is returned, and the task is moved
	// to the back of the queue; if there are no tasks to do, returns false.
	virtual bool do_one_task();

	// do tasks until there are none left; blocks until completion (even for multithreaded variants)
	virtual void flush();
};

class SingleThreadPool : public TaskPool{

	std::atomic_bool is_destructing = false;
	std::counting_semaphore<> empty_wait_sema {0};
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
	//size_t recycle_when_done_count = 0;

public:

	TaskCycle()=default;
	TaskCycle(const TaskCycle&)=delete;
	TaskCycle(TaskCycle&&)=delete;
	TaskCycle& operator=(const TaskCycle&)=delete;
	TaskCycle& operator=(TaskCycle&&)=delete;

	void recycle();
	//void recycle_when_done();
	size_t offhand_size() const;
	size_t total_size() const;

	// tries to do the next task; if the locks are obtained, the task is completed and then true is
	// returned; otherwise, the locks are released and false is returned, and the task is moved
	// to the back of the queue; if there are no tasks to do, returns false.
	virtual bool do_one_task() override;
};

class SingleThreadCycle : public TaskCycle{

	std::atomic_bool is_destructing = false;
	std::counting_semaphore<> empty_wait_sema {0};
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
