
#include <cstddef>
#include <random>
#include <chrono>
#include <vector>
#include <iterator>
#include <string>
#include <cassert>

using namespace std::literals;

namespace CT
{
	void init();

	template<typename T>
	std::string nameof(T&) { return typeid(T).name(); }

	template<typename T = void>
	struct fillup
	{
		template<typename... Args>
		void operator()(std::size_t, Args&...);
		static std::string name() { return "fillup"s; }
	};

	template<typename T = void>
	struct copy_to
	{
		template<typename C1>
		void operator()(const C1&) {}

		template<typename C1, typename C2, typename... Args>
		void operator()(const C1&, C2&, Args&...);

		static std::string name() { return "copy_to"s; }
	};

	template<typename T = void>
	struct compare
	{
		template<typename C1>
		bool operator()(const C1&) { return true; }

		template<typename C1, typename C2, typename... Args>
		bool operator()(const C1&, const C2&, const Args&...);

		static std::string name() { return "compare"s; }
	};

	template<typename T = void>
	struct insert_nth
	{
		void operator()(std::size_t, int) {}

		template<typename C1, typename... Args>
		void operator()(std::size_t, int, C1&, Args&...);

		static std::string name() { return "insert_nth"s; }
	};

	template<typename T = void>
	struct insert
	{
		template<typename C1, typename... Args>
		void operator()(C1&, Args&...);

		static std::string name() { return "insert"s; }
	};

	template<typename T = void>
	struct erase_nth
	{
		void operator()(std::size_t) {}

		template<typename C1, typename... Args>
		void operator()(std::size_t, C1&, Args&...);

		static std::string name() { return "erase_nth"s; }
	};

	template<typename T = void>
	struct erase
	{
		template<typename C1, typename... Args>
		void operator()(C1&, Args&...);

		static std::string name() { return "erase"s; }
	};

}

// ----------------------------------------------------------------------------

namespace CT
{
	namespace 
	{
		static bool inited = false;
		std::default_random_engine generator;
		std::uniform_int_distribution<int> distribution(1, 10'000);
	}
}

void CT::init()
{
	if (inited) return;
	inited = true;
	auto tm = std::chrono::system_clock::now();
	generator.seed((unsigned int)tm.time_since_epoch().count());
}

template<typename T>
template<typename... Args>
void CT::fillup<T>::operator()(std::size_t sz, Args&... args)
{
	init();
	std::vector<int> vi;
	for (auto i=0u; i<sz; ++i)
	{
		auto num = distribution(generator);
		vi.push_back(num);
	}
	copy_to<T>{}(vi, args...);
}

template<typename T>
template<typename C1, typename C2, typename... Args>
void CT::copy_to<T>::operator()(const C1& from, C2& first, Args&... rest)
{
	init();
#ifndef NDEBUG
	//std::cout << name() << " of " << nameof(first) << std::endl;
#endif
	for (auto&& x : from)
	{
		first.push_back(x);
	}
	copy_to<>{}(from, rest...);
}

template<typename T>
template<typename C1, typename C2, typename... Args>
bool CT::compare<T>::operator()(const C1& orig, const C2& first, const Args&... rest)
{
	init();
	auto i1 = orig.begin();
	auto i2 = first.begin();
	auto e1 = orig.end();
	auto e2 = first.end();
	while (true)
	{
		if ((i1==e1) && (i2==e2)) return true;
		if ((i1==e1) || (i2==e2)) return false;
		if (*i1 != *i2) return false;
		++i1; ++i2;
	}
	return compare<>{}(orig, rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::insert_nth<T>::operator()(std::size_t idx, int val, C1& first, Args&... rest)
{
	init();
#ifndef NDEBUG
	//std::cout << name() << " of " << nameof(first) << std::endl;
#endif
	auto itr = first.begin();
	std::advance(itr, idx);
	first.insert(itr, val);
	insert_nth<>{}(idx, val, rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::insert<T>::operator()(C1& first, Args&... rest)
{
	init();
	int sz = first.size();
	std::size_t idx = std::uniform_int_distribution<int>{0, sz}(generator);
	auto num = distribution(generator);
	insert_nth<>{}(idx, num, first, rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::erase_nth<T>::operator()(std::size_t idx, C1& first, Args&... rest)
{
	init();
#ifndef NDEBUG
	//std::cout << name() << " of " << nameof(first) << std::endl;
#endif
	assert(idx < first.size());
	auto itr = first.begin();
	std::advance(itr, idx);
	first.erase(itr);
	erase_nth<>{}(idx, rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::erase<T>::operator()(C1& first, Args&... rest)
{
	init();
	int sz = first.size();
	std::size_t idx = std::uniform_int_distribution<int>{0, sz-1}(generator);
	erase_nth<>{}(idx, first, rest...);
}

#ifdef STANDALONE

#include <list>
#include <iostream>

int main()
{
    using namespace std;
    using namespace CT;

    vector<int> vi;
    list<int> li;

    fillup<>{}(100, vi, li);
    insert<>{}(vi, li);
    bool ok = compare<>{}(vi, li);
    cout << boolalpha << ok << endl;
}

#endif

