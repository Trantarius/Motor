#include "TaskPool.hpp"
#include <cassert>

void TaskPool::add_tasks(const std::list<Task>& ts){
	std::scoped_lock lock(mtx);
	for(const Task& tk : ts){
		if(tk)
			tasks.push_back(tk);
	}
	task_count_sema.release(ts.size());
}

void TaskPool::add_tasks(std::list<Task>&& ts){
	std::scoped_lock lock(mtx);
	size_t count = ts.size();
	tasks.splice(tasks.end(), std::move(ts));
	task_count_sema.release(count);
}

void TaskPool::add_task(const Task& call){
#ifndef NDEBUG
	if(!(call))
		throw std::logic_error("invalid task");
#endif
	tasks.push_back(call);
	task_count_sema.release();
}
void TaskPool::add_task(Task&& call){
#ifndef NDEBUG
	if(!(call))
		throw std::logic_error("invalid task");
#endif
	tasks.push_back(std::move(call));
	task_count_sema.release();
}

size_t TaskPool::queue_size() const{
	std::scoped_lock lock(mtx);
	return tasks.size() + in_progress;
}

void TaskPool::do_one_task(){

	Task next;

	{
		std::scoped_lock lock(mtx);
		if(tasks.empty())
			return;
		next = std::move(tasks.front());
		tasks.pop_front();
		in_progress++;
	}

	TaskStatus status = next();

	bool is_empty = false;
	{
		std::scoped_lock lock(mtx);
		in_progress--;
		switch(status){
			case TASK_DONE:
			case TASK_EXPIRED:
				break;
			case TASK_DEFER:
				tasks.push_back(std::move(next));
				task_count_sema.release();
				break;
		}
		is_empty = tasks.size()==0 && in_progress==0;
	}

	if(is_empty)
		empty_notifier.notify_all();
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
		pool->task_count_sema.acquire();
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
		pool->task_count_sema.acquire();
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
	task_count_sema.release(threads.size());
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
	task_count_sema.release(count);
}

size_t TaskCycle::offhand_size() const{
	std::scoped_lock lock(mtx);
	return offhand.size();
}

size_t TaskCycle::total_size() const{
	std::scoped_lock lock(mtx);
	return tasks.size()+offhand.size();
}

void TaskCycle::do_one_task() {

	Task next;

	{
		std::scoped_lock lock(mtx);
		if(tasks.empty())
			return;
		next = std::move(tasks.front());
		tasks.pop_front();
		in_progress++;
	}

	TaskStatus status = next();

	bool is_empty = false;
	{
		std::scoped_lock lock(mtx);
		in_progress--;
		switch(status){
			case TASK_DONE:
				offhand.push_back(std::move(next));
				break;
			case TASK_EXPIRED:
				break;
			case TASK_DEFER:
				tasks.push_back(std::move(next));
				task_count_sema.release();
				break;
		}
		is_empty = tasks.size()==0 && in_progress==0;
	}

	if(is_empty)
		empty_notifier.notify_all();
}

void SingleThreadCycle::work_loop(SingleThreadCycle* pool){
	while(!pool->is_destructing){
		pool->task_count_sema.acquire();
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
		pool->task_count_sema.acquire();
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
	task_count_sema.release(threads.size());
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
