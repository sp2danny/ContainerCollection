
#include <cstddef>
#include <random>
#include <chrono>
#include <vector>
#include <iterator>
#include <string>
#include <cassert>
#include <iostream>

#include "container_operations.hpp"

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
		std::uniform_int_distribution<int> distribution(1, 100);
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

static const auto clr = "                                                  \r";

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
		#ifdef FULL_DIAG
		std::cerr << "attempting: "
			<< "push_back of "
			<< x << " into "
			<< nameof(first)
			<< clr;
		#endif
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

	#ifdef FULL_DIAG
	std::cerr << "attempting: "
		<< "insert_nth of "
		<< val << " into "
		<< nameof(first) << " (sz:" << first.size() << ")"
		<< " at pos " << idx
		<< clr;
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
	int sz = (int)first.size();
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
	typedef unsigned long long ULL;
	assert(ULL(idx) < ULL(first.size()));
	
	#ifdef FULL_DIAG
	std::cerr << "attempting: "
		<< "erase_nth in "
		<< nameof(first) << " (sz:" << first.size() << ")"
		<< " at pos " << idx
		<< clr;
	#endif

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
	int sz = (int)first.size();
	std::size_t idx = std::uniform_int_distribution<int>{0, sz-1}(generator);
	erase_nth<>{}(idx, first, rest...);
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

	sort(first);
	unique(first);
	sort_unique<>{}(rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::print<T>::operator()(std::ostream& out, C1& first, Args&... rest)
{
	out << nameof(first) << " : ";
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
	CO::remove(first, itm);
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
	auto itr1 = std::next(first.begin(), idx1);
	auto itr2 = std::next(first.begin(), idx2);
	C1 other;
	splice(first, itr1, itr2, other, other.begin());
	merge(first, other);
	splice_merge<>{}(rest...);
}

template<typename T>
template<typename Itm, typename C1, typename... Args>
void CT::binary_find_swap<T>::operator()(const Itm& itm1, const Itm& itm2, C1& first, Args&... rest)
{
	auto r1 = binary_find(first, itm1);
	auto r2 = binary_find(first, itm2);
	if (r1.first && r2.first)
	{
		using std::swap;
		swap(*r1.second, *r2.second);
	}
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

