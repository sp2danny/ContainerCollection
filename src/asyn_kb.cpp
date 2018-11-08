
#include "asyn_kb.h"

#include <pthread.h>

#include <cstdio>
#include <cassert>
#include <deque>

namespace AsynKB
{
	
	pthread_t getter_thread;
	pthread_mutex_t getter_lock;
	std::deque<char> getter_buffer;

	void* PerformWork(void*)
	{
		while (true)
		{
			int c = std::getchar();
			if (c<0) break;

			pthread_mutex_lock(&getter_lock);
			getter_buffer.push_back(c & 255);
			pthread_mutex_unlock(&getter_lock);
		}
		return nullptr;
	}

	void Start()
	{
		int result_code = pthread_create(&getter_thread, nullptr, &PerformWork, nullptr);
		assert(!result_code);
		result_code = pthread_mutex_init(&getter_lock, nullptr);
		assert(!result_code);
 	}

	bool HaveChar()
	{
		bool have;
		pthread_mutex_lock(&getter_lock);
		have = !getter_buffer.empty();
		pthread_mutex_unlock(&getter_lock);
		return have;
	}

	char GetChar()
	{
		char c;
		pthread_mutex_lock(&getter_lock);
		c = getter_buffer.front();
		getter_buffer.pop_front();
		pthread_mutex_unlock(&getter_lock);
		return c;
	}

	void Clear()
	{
		pthread_mutex_lock(&getter_lock);
		getter_buffer.clear();
		pthread_mutex_unlock(&getter_lock);
	}

}
