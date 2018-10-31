
#include <cstddef>
#include <random>
#include <chrono>
#include <vector>
#include <iterator>
#include <string>
#include <cassert>
#include <iostream>
#include <map>

#include "container_operations.hpp"

using namespace std::literals;

namespace CT
{
	inline void init();
	inline void start_clock();
	inline double stop_clock();
	template<typename Excl = void>
	inline void report_times();

	template<typename T>
	std::string nameof(T&&) { return typeid(T).name(); }

	template<typename T = void>
	struct fillup
	{
		template<typename... Args>
		void operator()(std::size_t, Args&...);
		static std::string name() { return "fillup"s; }
	};
	
	template<typename T = void>
	struct integrity
	{
		bool operator()() { return true; }
		template<typename C1, typename... Args>
		bool operator()(C1&, Args&...);
		static std::string name() { return "integrity"s; }
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
		insert(unsigned long count = 1) : count(count) {}
		template<typename C1, typename... Args>
		void operator()(C1&, Args&...);
	
		static std::string name() { return "insert"s; }
	private:
		unsigned long count;
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
		erase(unsigned long count = 1) : count(count) {}
		template<typename C1, typename... Args>
		void operator()(C1&, Args&...);

		static std::string name() { return "erase"s; }
	private:
		unsigned long count;
	};

	template<typename T = void>
	struct sort
	{
		void operator()() {}

		template<typename C1, typename... Args>
		void operator()(C1&, Args&...);

		static std::string name() { return "sort"s; }
	};

	template<typename T = void>
	struct unique
	{
		void operator()() {}

		template<typename C1, typename... Args>
		void operator()(C1&, Args&...);

		static std::string name() { return "unique"s; }
	};

	template<typename T = void>
	struct sort_unique
	{
		void operator()() {}

		template<typename C1, typename... Args>
		void operator()(C1&, Args&...);

		static std::string name() { return "sort_unique"s; }
	};

	template<typename T = void>
	struct print
	{
		void operator()(std::ostream&) {}

		template<typename C1, typename... Args>
		void operator()(std::ostream&, C1&, Args&...);

		static std::string name() { return "print"s; }
	};

	template<typename T = void>
	struct remove
	{
		template<typename Itm>
		void operator()(const Itm&) {}

		template<typename Itm, typename C1, typename... Args>
		void operator()(const Itm&, C1&, Args&...);

		static std::string name() { return "remove"s; }
	};

	template<typename T = void>
	struct splice_merge
	{
		void operator()() {}

		template<typename C1, typename... Args>
		void operator()(C1&, Args&...);

		static std::string name() { return "splice_merge"s; }
	};
	
	template<typename T = void>
	struct binary_find_swap
	{
		template<typename Itm>
		void operator()(const Itm&, const Itm&) {}

		template<typename Itm, typename C1, typename... Args>
		void operator()(const Itm&, const Itm&, C1&, Args&...);

		static std::string name() { return "binary_find_swap"s; }
	};

}

// ----------------------------------------------------------------------------

namespace CT
{
	namespace 
	{
		static bool inited = false;
		std::default_random_engine generator;
		std::uniform_int_distribution<int> distribution(1, 1000);
		std::chrono::high_resolution_clock::time_point t1;
		std::map<std::string, double> time_data;
	}
}

inline void CT::init()
{
	if (inited) return;
	inited = true;
	auto tm = std::chrono::system_clock::now();
	generator.seed((unsigned int)tm.time_since_epoch().count());
}

inline void CT::start_clock()
{
	t1 = std::chrono::high_resolution_clock::now();
}

inline double CT::stop_clock()
{
	std::chrono::duration<double, std::ratio<1,1>> diff = std::chrono::high_resolution_clock::now() - t1;
	return diff.count();
}

template<typename Excl>
inline void CT::report_times()
{
	for (auto&& x : time_data)
	{
		if (nameof(Excl{}) != x.first)
			std::cout << x.first << " : " << x.second << " s\n";
	}
}

template<typename T>
template<typename C1, typename... Args>
bool CT::integrity<T>::operator()(C1& first, Args&... rest)
{
	if (!CO::integrity(first))
	{
		std::cerr << "\nintegrity failedd for " << nameof(first) << "\n";
		return false;
	}
	return CT::integrity<>{}(rest...);
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

static const auto clr = "                                                  \r";

template<typename T>
template<typename C1, typename C2, typename... Args>
void CT::copy_to<T>::operator()(const C1& from, C2& first, Args&... rest)
{
	init();
	#ifndef NDEBUG
	//std::cout << name() << " of " << nameof(first) << std::endl;
	#endif
	start_clock();
	for (auto&& x : from)
	{
		#ifdef FULL_DIAG
		std::cerr << "attempting: "
			<< "push_back of "
			<< x << " into "
			<< nameof(first)
			<< clr;
		#endif
		first.push_back(x);
	}
	time_data[nameof(first)] += stop_clock();
	copy_to<>{}(from, rest...);
}

template<typename T>
template<typename C1, typename C2, typename... Args>
bool CT::compare<T>::operator()(const C1& orig, const C2& first, const Args&... rest)
{
	init();
	start_clock();
	auto i1 = orig.begin();
	auto i2 = first.begin();
	auto e1 = orig.end();
	auto e2 = first.end();
	while (true)
	{
		if ((i1==e1) && (i2==e2)) break;
		if ((i1==e1) || (i2==e2)) return false;
		if (*i1 != *i2) return false;
		++i1; ++i2;
	}
	time_data[nameof(first)] += stop_clock();
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

	#ifdef FULL_DIAG
	std::cerr << "attempting: "
		<< "insert_nth of "
		<< val << " into "
		<< nameof(first) << " (sz:" << first.size() << ")"
		<< " at pos " << idx
		<< clr;
	#endif

	start_clock();
	auto itr = first.begin();
	std::advance(itr, idx);
	first.insert(itr, val);
	time_data[nameof(first)] += stop_clock();
	insert_nth<>{}(idx, val, rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::insert<T>::operator()(C1& first, Args&... rest)
{
	init();
	for (auto i = 0ul; i<count; ++i)
	{
		int sz = (int)first.size();
		std::size_t idx = std::uniform_int_distribution<int>{0, sz}(generator);
		auto num = distribution(generator);
		insert_nth<>{}(idx, num, first, rest...);
	}
}

template<typename T>
template<typename C1, typename... Args>
void CT::erase_nth<T>::operator()(std::size_t idx, C1& first, Args&... rest)
{
	init();
	#ifndef NDEBUG
	//std::cout << name() << " of " << nameof(first) << std::endl;
	#endif
	typedef unsigned long long ULL;
	assert(ULL(idx) < ULL(first.size()));
	
	#ifdef FULL_DIAG
	std::cerr << "attempting: "
		<< "erase_nth in "
		<< nameof(first) << " (sz:" << first.size() << ")"
		<< " at pos " << idx
		<< clr;
	#endif

	start_clock();
	auto itr = first.begin();
	std::advance(itr, idx);
	first.erase(itr);
	time_data[nameof(first)] += stop_clock();
	erase_nth<>{}(idx, rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::erase<T>::operator()(C1& first, Args&... rest)
{
	init();
	for (auto i = 0ul; i<count; ++i)
	{
		int sz = (int)first.size();
		std::size_t idx = std::uniform_int_distribution<int>{0, sz-1}(generator);
		erase_nth<>{}(idx, first, rest...);
	}
}

template<typename T>
template<typename C1, typename... Args>
void CT::sort_unique<T>::operator()(C1& first, Args&... rest)
{
	#ifdef FULL_DIAG
	std::cerr << "attempting: "
		<< "sort_unique of "
		<< nameof(first) << " (sz:" << first.size() << ")"
		<< clr;
	#endif

	start_clock();
	CO::sort(first);
	CO::unique(first);
	time_data[nameof(first)] += stop_clock();
	sort_unique<>{}(rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::sort<T>::operator()(C1& first, Args&... rest)
{
#ifdef FULL_DIAG
	std::cerr << "attempting: "
		<< "sort of "
		<< nameof(first) << " (sz:" << first.size() << ")"
		<< clr;
#endif

	start_clock();
	CO::sort(first);
	time_data[nameof(first)] += stop_clock();
	CT::sort<>{}(rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::unique<T>::operator()(C1& first, Args&... rest)
{
#ifdef FULL_DIAG
	std::cerr << "attempting: "
		<< "unique of "
		<< nameof(first) << " (sz:" << first.size() << ")"
		<< clr;
#endif
	start_clock();
	CO::unique(first);
	time_data[nameof(first)] += stop_clock();
	CT::unique<>{}(rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::print<T>::operator()(std::ostream& out, C1& first, Args&... rest)
{
	out << nameof(first) << "\n";
	for (auto&& x : first)
	{
		out << x << ' ';
	}
	out << std::endl;
	print<>{}(out, rest...);
}

template<typename T>
template<typename Itm, typename C1, typename... Args>
void CT::remove<T>::operator()(const Itm& itm, C1& first, Args&... rest)
{
	start_clock();
	CO::remove(first, itm);
	time_data[nameof(first)] += stop_clock();
	CT::remove<>{}(itm, rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::splice_merge<T>::operator()(C1& first, Args&... rest)
{
	init();
	int sz = (int)first.size();
	std::size_t idx1 = std::uniform_int_distribution<int>{0, sz-1}(generator);
	std::size_t idx2 = std::uniform_int_distribution<int>{0, sz-1}(generator);
	if (idx1>idx2) std::swap(idx1, idx2);
	start_clock();
	auto itr1 = nth(first, idx1);
	auto itr2 = nth(first, idx2);
	C1 other;
	splice(first, itr1, itr2, other, other.begin());
	merge(first, other);
	time_data[nameof(first)] += stop_clock();
	splice_merge<>{}(rest...);
}

template<typename T>
template<typename Itm, typename C1, typename... Args>
void CT::binary_find_swap<T>::operator()(const Itm& itm1, const Itm& itm2, C1& first, Args&... rest)
{
	start_clock();
	auto r1 = binary_find(first, itm1);
	auto r2 = binary_find(first, itm2);
	if (r1.first && r2.first)
	{
		using std::swap;
		swap(*r1.second, *r2.second);
	}
	time_data[nameof(first)] += stop_clock();
	binary_find_swap<>{}(itm1, itm2, rest...);
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

