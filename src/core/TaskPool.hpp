#pragma once
#include <memory>
#include <mutex>
#include <thread>
#include <semaphore>
#include <list>
#include <functional>
#include <condition_variable>

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
	std::function<void(void)> call;
	std::list<std::shared_ptr<TaskLock::Core>> required_locks;
	friend class TaskPool;
public:
	void add_lock(const TaskLock&);
	bool attempt();

	Task()=default;
	Task(const std::function<void(void)>& f) : call(f) {}
	Task(std::function<void(void)>&& f) : call(std::move(f)) {}
};

class TaskPool{
protected:

	mutable std::mutex mtx;
	std::list<std::unique_ptr<Task>> tasks;
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

	virtual void add_task(std::unique_ptr<Task>&&);
	void add_task(const Task& call);
	void add_task(Task&& call);
	void add_task(const std::function<void(void)>& call);
	void add_task(std::function<void(void)>&& call);
	size_t queue_size() const;
	bool empty() const;

	// tries to do the next task; if the locks are obtained, the task is completed and then true is
	// returned; otherwise, the locks are released and false is returned, and the task is moved
	// to the back of the queue; if there are no tasks to do, returns false.
	bool do_one_task();

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

class ThreadPool : public TaskPool{

	bool is_destructing = false;
	std::list<std::thread> threads;

	static void work_loop(ThreadPool*);
	using TaskPool::do_one_task;

public:

	ThreadPool();
	~ThreadPool();
	void flush() override;
};

