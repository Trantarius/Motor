#pragma once
#include <atomic>
#include <thread>


class SpinLock {
  std::atomic_flag locked = ATOMIC_FLAG_INIT;

public:
  void lock() {
    while (locked.test_and_set(std::memory_order_acquire)) {
      //try again
    }
  }
  void unlock() {
    locked.clear(std::memory_order_release);
  }
  bool try_lock(){
    return !locked.test_and_set(std::memory_order_acquire);
  }
};

class YieldLock {
  std::atomic_flag locked = ATOMIC_FLAG_INIT;

public:
  void lock() {
    while (locked.test_and_set(std::memory_order_acquire)) {
      std::this_thread::yield();
    }
  }
  void unlock() {
    locked.clear(std::memory_order_release);
  }
  bool try_lock(){
    return !locked.test_and_set(std::memory_order_acquire);
  }
};
