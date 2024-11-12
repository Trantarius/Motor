#include "TaskPool.hpp"


bool TaskLock::_is_owned_by_me(const std::shared_ptr<Core>& core){
	size_t me = std::hash<std::thread::id>()(std::this_thread::get_id());
	size_t owner = std::atomic_load(&(core->owner));
	return me==owner;
}

bool TaskLock::_is_owned_by_anyone(const std::shared_ptr<Core>& core){
	size_t no_one = std::hash<std::thread::id>()(std::thread::id());
	size_t owner = std::atomic_load(&(core->owner));
	return owner!=no_one;
}

bool TaskLock::_try_lock(const std::shared_ptr<Core>& core){
	size_t no_one = std::hash<std::thread::id>()(std::thread::id());
	size_t me = std::hash<std::thread::id>()(std::this_thread::get_id());
	bool locked = std::atomic_compare_exchange_strong(&(core->owner), &no_one, me);
	if(locked)
		core->depth++;
	return locked;
}

void TaskLock::_unlock(const std::shared_ptr<Core>& core){
#ifndef NDEBUG
	if(!_is_owned_by_me(core))
		throw std::logic_error("Task lock was unlocked by a thread that doesn't own it");
#endif
	core->depth--;
	if(core->depth==0){
		size_t no_one = std::hash<std::thread::id>()(std::thread::id());
		std::atomic_store(&(core->owner), no_one);
	}
}

TaskLock::~TaskLock(){
#ifndef NDEBUG
	if(is_owned_by_anyone())
		throw std::logic_error("Task lock was destroyed while still locked");
#endif
}


void Task::add_lock(const TaskLock& lk){
	required_locks.push_back(lk.core);
}

bool Task::attempt(){
	struct multiple_lock{
		std::list<std::shared_ptr<TaskLock::Core>> locks;
		~multiple_lock(){
			for(auto it=locks.rbegin();it!=locks.rend();it++){
				TaskLock::_unlock(*it);
			}
		}
	} mlock;

	{

		auto iter = required_locks.begin();
		for(;iter!=required_locks.end();iter++){
			if(TaskLock::_try_lock(*iter)){
				mlock.locks.push_back(*iter);
			}else{
				return false;
			}
		}
	}

	(call)();

	return true;
}

void TaskPool::add_task(std::unique_ptr<Task>&& task){
#ifndef NDEBUG
	if(!task)
		throw std::logic_error("cannot add null task");
	if(!(task->call))
		throw std::logic_error("cannot add a task without a callable");
#endif
	std::scoped_lock lock(mtx);
	tasks.push_back(task);
	empty_wait_sema.release();
}

void TaskPool::add_task(const Task& call){
	add_task(std::make_unique<Task>(call));
}
void TaskPool::add_task(Task&& call){
	add_task(std::make_unique<Task>(std::move(call)));
}

void TaskPool::add_task(const std::function<void(void)>& call){
	add_task(std::make_unique<Task>(call));
}
void TaskPool::add_task(std::function<void(void)>&& call){
	add_task(std::make_unique<Task>(std::move(call)));
}

size_t TaskPool::queue_size() const{
	std::scoped_lock lock(mtx);
	return tasks.size();
}

bool TaskPool::empty() const{
	std::scoped_lock lock(mtx);
	return tasks.size()==0 && in_progress==0;
}

bool TaskPool::do_one_task(){

	std::unique_ptr<Task> next;

	{
		std::scoped_lock lock(mtx);
		if(tasks.empty())
			return false;
		next = std::move(tasks.front());
		tasks.pop_front();
		in_progress++;
	}

	bool success = next->attempt();

	bool is_empty = false;
	{
		std::scoped_lock lock(mtx);
		in_progress--;
		if(!success)
			tasks.push_back(std::move(next));
		is_empty = tasks.size()==0 && in_progress==0;
	}

	if(is_empty)
		empty_notifier.notify_all();

	return success;
}

void TaskPool::flush(){
	while(!empty()){
		do_one_task();
	}
}

TaskPool::~TaskPool(){
#ifndef NDEBUG
	if(!empty()){
		throw std::logic_error("Task pool deleted with "+std::to_string(queue_size())+" tasks remaining");
	}
#endif
}


void SingleThreadPool::work_loop(SingleThreadPool& pool){
	while(!pool.is_destructing){
		pool.empty_wait_sema.acquire();
		pool.do_one_task();
	}
}

SingleThreadPool::SingleThreadPool() : thread(work_loop, *this) {}

SingleThreadPool::~SingleThreadPool(){
#ifndef NDEBUG
	if(!empty()){
		throw std::logic_error("Task pool deleted with "+std::to_string(queue_size())+" tasks remaining");
	}
#endif
	is_destructing = true;
	thread.join();
}

void SingleThreadPool::flush(){
	std::unique_lock lk(mtx);
	while(!(tasks.empty() && in_progress==0)){
		empty_notifier.wait(lk);
	}
}


void ThreadPool::work_loop(ThreadPool& pool){
	while(!pool.is_destructing){
		pool.empty_wait_sema.acquire();
		pool.do_one_task();
	}
}

ThreadPool::ThreadPool() {
	size_t thread_count = std::thread::hardware_concurrency();
	thread_count = thread_count > 4 ? thread_count : 4;
	for(int n=0;n<thread_count;n++){
		threads.emplace_back(&work_loop, *this);
	}
}

ThreadPool::~ThreadPool(){
#ifndef NDEBUG
	if(!empty()){
		throw std::logic_error("Task pool deleted with "+std::to_string(queue_size())+" tasks remaining");
	}
#endif
	is_destructing = true;
	empty_wait_sema.release(threads.size());
	for( std::thread& t : threads){
		t.join();
	}
}

void ThreadPool::flush(){
	std::unique_lock lk(mtx);
	while(!(tasks.empty() && in_progress==0)){
		empty_notifier.wait(lk);
	}
}
