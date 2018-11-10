
#include "asyn_kb.h"

#include <cstdio>
#include <cassert>
#include <deque>

#include <thread>
#include <mutex>
#include <chrono>

using namespace std::literals;

namespace AsynKB
{
	std::thread getter_thread;
	std::mutex getter_lock;
	std::deque<char> getter_buffer;

	void PerformWork()
	{
		while (true)
		{
			int c = std::getchar();
			if (c<0) break;

			getter_lock.lock();
			getter_buffer.push_back(c & 255);
			getter_lock.unlock();
		}
		return;
	}

	void Start()
	{
		getter_thread = std::thread(PerformWork);
 	}

	bool HaveChar()
	{
		bool have;
		getter_lock.lock();
		have = !getter_buffer.empty();
		getter_lock.unlock();
		return have;
	}

	char GetChar()
	{
		char c;
		while (true)
		{
			getter_lock.lock();
			bool have = !getter_buffer.empty();
			if (have)
			{
				c = getter_buffer.front();
				getter_buffer.pop_front();
			}
			getter_lock.unlock();
			if (have)
				break;
			else
				std::this_thread::sleep_for(150ms);
		}
		return c;
	}
	
	void WaitChar()
	{
		while (true)
		{
			getter_lock.lock();
			bool have = !getter_buffer.empty();
			getter_lock.unlock();
			if (have)
				break;
			else
				std::this_thread::sleep_for(150ms);
		}
	}

	void Clear()
	{
		getter_lock.lock();
		getter_buffer.clear();
		getter_lock.unlock();
	}

	void Stop()
	{
		std::putchar(-1);

		//getter_lock.lock();
		//getter_thread.join();
	}
}




