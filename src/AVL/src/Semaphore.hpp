
#pragma once

#include <atomic>
#include <vector>

#include <list>

class Semaphore
{
public:
	Semaphore(int i) : count(i), lock(0) {}
	template<typename F>
	void wait( F&& func );
	void signal();
private:
	std::atomic<int> count;
	std::atomic<bool> lock;
	typedef void (*FP)();
	std::vector<FP> callbacks;
};

