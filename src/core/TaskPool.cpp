#include "TaskPool.hpp"
#include <cassert>


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
	assert(!is_owned_by_anyone());
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

	call.try_call();

	return true;
}

void TaskPool::add_tasks(const std::list<Task>& ts){
	std::scoped_lock lock(mtx);
	for(const Task& tk : ts){
		#ifndef NDEBUG
			if(!(tk.call))
				throw std::logic_error("cannot add a task without a callable");
		#endif
		tasks.push_back(tk);
	}
	empty_wait_sema.release(ts.size());
}

void TaskPool::add_tasks(std::list<Task>&& ts){
	std::scoped_lock lock(mtx);
	size_t count = ts.size();
	tasks.splice(tasks.end(), std::move(ts));
	empty_wait_sema.release(count);
}

void TaskPool::add_tasks(const std::list<CallablePtr<void(void)>>& ts){
	std::scoped_lock lock(mtx);
	for(const CallablePtr<void(void)>& tk : ts){
		#ifndef NDEBUG
			if(!(tk))
				throw std::logic_error("cannot add a task without a callable");
		#endif
		tasks.push_back(tk);
	}
	empty_wait_sema.release(ts.size());
}

void TaskPool::add_task(const Task& call){
#ifndef NDEBUG
	if(!(call.call))
		throw std::logic_error("cannot add a task without a callable");
#endif
	tasks.push_back(call);
	empty_wait_sema.release();
}
void TaskPool::add_task(Task&& call){
#ifndef NDEBUG
	if(!(call.call))
		throw std::logic_error("cannot add a task without a callable");
#endif
	tasks.push_back(std::move(call));
	empty_wait_sema.release();
}

size_t TaskPool::queue_size() const{
	std::scoped_lock lock(mtx);
	return tasks.size() + in_progress;
}

bool TaskPool::do_one_task(){

	Task next;

	{
		std::scoped_lock lock(mtx);
		if(tasks.empty())
			return false;
		next = std::move(tasks.front());
		tasks.pop_front();
		in_progress++;
	}

	bool success = next.attempt();

	bool is_empty = false;
	{
		std::scoped_lock lock(mtx);
		in_progress--;
		if(!success){
			tasks.push_back(std::move(next));
			empty_wait_sema.release();
		}
		is_empty = tasks.size()==0 && in_progress==0;
	}

	if(is_empty)
		empty_notifier.notify_all();

	return success;
}

void TaskPool::flush(){
	while(queue_size()!=0){
		do_one_task();
	}
}

TaskPool::~TaskPool(){
	assert(queue_size()==0);
}


void SingleThreadPool::work_loop(SingleThreadPool* pool){
	while(!pool->is_destructing){
		pool->empty_wait_sema.acquire();
		pool->do_one_task();
	}
}

SingleThreadPool::SingleThreadPool() : thread(&work_loop, this) {}

SingleThreadPool::~SingleThreadPool(){
	assert(queue_size()==0);
	is_destructing = true;
	thread.join();
}

void SingleThreadPool::flush(){
	std::unique_lock lk(mtx);
	while(!(tasks.empty() && in_progress==0)){
		empty_notifier.wait(lk);
	}
}


void MultiThreadPool::work_loop(MultiThreadPool* pool){
	while(!pool->is_destructing){
		pool->empty_wait_sema.acquire();
		pool->do_one_task();
	}
}

MultiThreadPool::MultiThreadPool() {
	size_t thread_count = std::thread::hardware_concurrency();
	thread_count = thread_count > 4 ? thread_count : 4;
	for(int n=0;n<thread_count;n++){
		threads.emplace_back(&work_loop, this);
	}
}

MultiThreadPool::~MultiThreadPool(){
	assert(queue_size()==0);
	is_destructing = true;
	empty_wait_sema.release(threads.size());
	for( std::thread& t : threads){
		t.join();
	}
}

void MultiThreadPool::flush(){
	std::unique_lock lk(mtx);
	while(!(tasks.empty() && in_progress==0)){
		empty_notifier.wait(lk);
	}
}


void TaskCycle::recycle(){
	std::scoped_lock lock(mtx);
	size_t count = offhand.size();
	tasks.splice(tasks.end(), std::move(offhand));
	empty_wait_sema.release(count);
}
/*
void TaskCycle::recycle_when_done(){
	std::scoped_lock lock(mtx);
	if(tasks.size()+in_progress == 0){
		size_t count = offhand.size();
		tasks.splice(tasks.end(), std::move(offhand));
		empty_wait_sema.release(count);
	}else{
		recycle_when_done_count++;
	}
}
*/

size_t TaskCycle::offhand_size() const{
	std::scoped_lock lock(mtx);
	return offhand.size();
}

size_t TaskCycle::total_size() const{
	std::scoped_lock lock(mtx);
	return tasks.size()+offhand.size();
}

bool TaskCycle::do_one_task() {
	Task next;

	{
		std::scoped_lock lock(mtx);
		if(tasks.empty())
			return false;
		next = std::move(tasks.front());
		tasks.pop_front();
		in_progress++;
	}

	bool success = next.attempt();

	bool is_empty = false;
	{
		std::scoped_lock lock(mtx);
		in_progress--;
		if(!success){
			tasks.push_back(std::move(next));
			empty_wait_sema.release();
		}
		else{
			offhand.push_back(std::move(next));
		}
		is_empty = tasks.size()==0 && in_progress==0;
		//if(is_empty && recycle_when_done_count>0){
		//	size_t count = offhand.size();
		//	tasks.splice(tasks.end(), std::move(offhand));
		//	empty_wait_sema.release(count);
		//	recycle_when_done_count--;
		//}
	}

	if(is_empty)
		empty_notifier.notify_all();

	return success;
}

void SingleThreadCycle::work_loop(SingleThreadCycle* pool){
	while(!pool->is_destructing){
		pool->empty_wait_sema.acquire();
		pool->do_one_task();
	}
}

SingleThreadCycle::SingleThreadCycle() : thread(&work_loop, this) {}

SingleThreadCycle::~SingleThreadCycle(){
	assert(queue_size()==0);
	is_destructing = true;
	thread.join();
}

void SingleThreadCycle::flush(){
	std::unique_lock lk(mtx);
	while(!(tasks.empty() && in_progress==0)){
		empty_notifier.wait(lk);
	}
}

void MultiThreadCycle::work_loop(MultiThreadCycle* pool){
	while(!pool->is_destructing){
		pool->empty_wait_sema.acquire();
		pool->do_one_task();
	}
}

MultiThreadCycle::MultiThreadCycle() {
	size_t thread_count = std::thread::hardware_concurrency();
	thread_count = thread_count > 4 ? thread_count : 4;
	for(int n=0;n<thread_count;n++){
		threads.emplace_back(&work_loop, this);
	}
}

MultiThreadCycle::~MultiThreadCycle(){
	assert(queue_size()==0);
	is_destructing = true;
	empty_wait_sema.release(threads.size());
	for( std::thread& t : threads){
		t.join();
	}
}

void MultiThreadCycle::flush(){
	std::unique_lock lk(mtx);
	while(!(tasks.empty() && in_progress==0)){
		empty_notifier.wait(lk);
	}
}
