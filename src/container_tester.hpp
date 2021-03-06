
#include <cassert>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

#include "container_operations.hpp"

using namespace std::literals;

namespace CT
{
extern void   init();
extern void   start_clock();
extern double stop_clock();
template<typename Excl = void*>
void report_times(double = 1.0, std::string = "s");
void clear_times();

template<typename T>
std::string nameof(const T&)
{
	return typeid(T).name();
}

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
	void operator()(const C1&)
	{
	}

	template<typename C1, typename C2, typename... Args>
	void operator()(const C1&, C2&, Args&...);

	static std::string name() { return "copy_to"s; }
};

template<typename T = void>
struct compare
{
	template<typename C1>
	bool operator()(const C1&)
	{
		return true;
	}

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
	insert(std::size_t count = 1) : count(count) {}
	template<typename C1, typename... Args>
	void operator()(C1&, Args&...);

	static std::string name() { return "insert"s; }

private:
	std::size_t count;
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
	erase(std::size_t count = 1) : count(count) {}
	template<typename C1, typename... Args>
	void operator()(C1&, Args&...);

	static std::string name() { return "erase"s; }

private:
	std::size_t count;
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

template<typename T = void>
struct nth_swap
{
	nth_swap(std::size_t count = 1) : count(count) {}

	void swp(std::size_t, std::size_t) {}

	template<typename C1, typename... Args>
	void swp(std::size_t, std::size_t, C1&, Args&...);

	template<typename C1, typename... Args>
	void operator()(C1&, Args&...);

	static std::string name() { return "nth_swap"s; }

private:
	std::size_t count;
};

template<typename T = void>
struct reverse
{
	void operator()() {}
	template<typename C1, typename... Args>
	void operator()(C1&, Args&...);
};

template<typename T = void>
struct clear
{
	void operator()() {}
	template<typename C1, typename... Args>
	void operator()(C1&, Args&...);
};

template<typename T = void>
struct size
{
	size(std::size_t count) : count(count) {}
	bool operator()() { return true; }
	template<typename C1, typename... Args>
	bool operator()(C1&, Args&...);

private:
	std::size_t count;
};

} // namespace CT

// ----------------------------------------------------------------------------

namespace CT
{
extern std::map<std::string, std::map<std::string, double>> time_data;
extern std::default_random_engine generator;

struct ListItem
{
	std::string name;
	std::string time;
	int name_ln;
	int bef_dot;
	int aft_dot;
};
inline std::string space(int i)
{
	if (i < 0)
		return "";
	std::string str;
	while (i--)
		str += " ";
	return str;
}
} // namespace CT

template<typename Excl>
void CT::report_times(double multiplyer, std::string unit)
{
	for (auto&& x : time_data)
	{
		if (nameof(Excl{}) != x.first)
		{
			std::cout << "Container : " << x.first << std::endl;
			double sum = 0.0;
			std::vector<ListItem> vli;
			for (auto&& y : x.second)
			{
				ListItem li;
				li.name    = y.first;
				li.name_ln = y.first.size();
				std::stringstream ss;
				ss << y.second;
				li.time  = ss.str();
				auto pos = li.time.find('.');
				if (pos == std::string::npos)
				{
					li.time += ".0";
					pos = li.time.find('.');
				}
				li.bef_dot = pos;
				li.aft_dot = li.time.size() - pos - 1;
				vli.push_back(li);
				sum += y.second;
			}
			int sz = vli.size();
			if (sz)
			{
				int max_name_ln = vli[0].name_ln;
				int max_bef_dot = vli[0].bef_dot;
				int max_aft_dot = vli[0].aft_dot;
				for (int i = 1; i < sz; ++i)
				{
					if (vli[i].name_ln > max_name_ln)
						max_name_ln = vli[i].name_ln;
					if (vli[i].bef_dot > max_bef_dot)
						max_bef_dot = vli[i].bef_dot;
					if (vli[i].aft_dot > max_aft_dot)
						max_aft_dot = vli[i].aft_dot;
				}
				for (auto&& li : vli)
				{
					std::cout << "    " << li.name << space(max_name_ln - li.name_ln) << " : "
							  << space(max_bef_dot - li.bef_dot) << li.time << space(max_aft_dot - li.aft_dot) << " ms\n";
				}
				std::cout << "Container totals : " << sum * multiplyer / 1000.0 << " " << unit << "\n\n";
			}
		}
	}
}

template<typename T>
template<typename C1, typename... Args>
bool CT::integrity<T>::operator()(C1& first, Args&... rest)
{
	if (!CO::integrity(first))
	{
		std::cerr << "\nintegrity failed for " << nameof(first) << "\n";
		return false;
	}
	return CT::integrity<>{}(rest...);
}

template<typename T>
template<typename... Args>
void CT::fillup<T>::operator()(std::size_t sz, Args&... args)
{
	init();
	std::uniform_int_distribution<int> distribution(1, (int)sz);
	std::vector<int> vi;
	for (auto i = 0u; i < sz; ++i)
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
// std::cout << name() << " of " << nameof(first) << std::endl;
#endif
	start_clock();
	for (auto&& x : from)
	{
#ifdef FULL_DIAG
		std::cerr << "attempting: "
				  << "push_back of " << x << " into " << nameof(first) << clr;
#endif
		first.push_back(x);
	}
	time_data[nameof(first)][name()] += stop_clock();
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
	bool eq = true;
	while (true)
	{
		if ((i1 == e1) && (i2 == e2))
			break; // both at end, containers equal
		if ((i1 == e1) || (i2 == e2))
		{
			eq = false;
			break;
		} // one at end, containers not equal
		if (*i1 != *i2)
		{
			eq = false;
			break;
		} // items differ, containers not equal
		++i1;
		++i2; // next items
	}
	time_data[nameof(first)][name()] += stop_clock();
	return compare<>{}(orig, rest...) && eq;
}

template<typename T>
template<typename C1, typename... Args>
void CT::insert_nth<T>::operator()(std::size_t idx, int val, C1& first, Args&... rest)
{
	init();
#ifndef NDEBUG
// std::cout << name() << " of " << nameof(first) << std::endl;
#endif

#ifdef FULL_DIAG
	std::cerr << "attempting: "
			  << "insert_nth of " << val << " into " << nameof(first) << " (sz:" << first.size() << ")"
			  << " at pos " << idx << clr;
#endif

	start_clock();
	auto itr = CO::nth(first, idx);
	first.insert(itr, val);
	time_data[nameof(first)][name()] += stop_clock();
	insert_nth<>{}(idx, val, rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::insert<T>::operator()(C1& first, Args&... rest)
{
	init();
	for (std::size_t i = 0; i < count; ++i)
	{
		auto sz = first.size();
		std::uniform_int_distribution<int> distribution(1, (int)count);
		std::size_t idx = std::uniform_int_distribution<std::size_t>{0, sz}(generator);
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
	// std::cout << name() << " of " << nameof(first) << std::endl;
	typedef unsigned long long ULL;
	assert(ULL(idx) < ULL(first.size()));
#endif

#ifdef FULL_DIAG
	std::cerr << "attempting: "
			  << "erase_nth in " << nameof(first) << " (sz:" << first.size() << ")"
			  << " at pos " << idx << clr;
#endif

	start_clock();
	auto itr = CO::nth(first, idx);
	first.erase(itr);
	time_data[nameof(first)][name()] += stop_clock();
	erase_nth<>{}(idx, rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::erase<T>::operator()(C1& first, Args&... rest)
{
	init();
	for (auto i = 0ul; i < count; ++i)
	{
		std::size_t sz  = first.size();
		std::size_t idx = std::uniform_int_distribution<std::size_t>{0, sz - 1}(generator);
		erase_nth<>{}(idx, first, rest...);
	}
}

template<typename T>
template<typename C1, typename... Args>
void CT::sort_unique<T>::operator()(C1& first, Args&... rest)
{
#ifdef FULL_DIAG
	std::cerr << "attempting: "
			  << "sort_unique of " << nameof(first) << " (sz:" << first.size() << ")" << clr;
#endif

	start_clock();
	CO::sort(first);
	CO::unique(first);
	time_data[nameof(first)][name()] += stop_clock();
	sort_unique<>{}(rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::sort<T>::operator()(C1& first, Args&... rest)
{
#ifdef FULL_DIAG
	std::cerr << "attempting: "
			  << "sort of " << nameof(first) << " (sz:" << first.size() << ")" << clr;
#endif

	start_clock();
	CO::sort(first);
	time_data[nameof(first)][name()] += stop_clock();
	CT::sort<>{}(rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::unique<T>::operator()(C1& first, Args&... rest)
{
#ifdef FULL_DIAG
	std::cerr << "attempting: "
			  << "unique of " << nameof(first) << " (sz:" << first.size() << ")" << clr;
#endif
	start_clock();
	CO::unique(first);
	time_data[nameof(first)][name()] += stop_clock();
	CT::unique<>{}(rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::print<T>::operator()(std::ostream& out, C1& first, Args&... rest)
{
	out << nameof(first) << "\n";
	out << "(" << first.size() << ")";
	for (auto&& x : first)
	{
		out << ' ' << x;
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
	time_data[nameof(first)][name()] += stop_clock();
	CT::remove<>{}(itm, rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::splice_merge<T>::operator()(C1& first, Args&... rest)
{
	init();
	int sz = (int)first.size();
	std::size_t idx1 = std::uniform_int_distribution<int>{0, sz - 1}(generator);
	std::size_t idx2 = std::uniform_int_distribution<int>{0, sz - 1}(generator);
	if (idx1 > idx2)
		std::swap(idx1, idx2);
	start_clock();
	auto itr1 = nth(first, idx1);
	auto itr2 = nth(first, idx2);
	C1 other;
	splice(first, itr1, itr2, other, other.begin());
	merge(first, other);
	time_data[nameof(first)][name()] += stop_clock();
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
	time_data[nameof(first)][name()] += stop_clock();
	binary_find_swap<>{}(itm1, itm2, rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::nth_swap<T>::swp(std::size_t idx1, std::size_t idx2, C1& first, Args&... rest)
{
	start_clock();
	auto itr1 = nth(first, idx1);
	auto itr2 = nth(first, idx2);
	using std::swap;
	std::swap(*itr1, *itr2);
	time_data[nameof(first)][name()] += stop_clock();
	swp(idx1, idx2, rest...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::nth_swap<T>::operator()(C1& first, Args&... rest)
{
	init();
	int sz = (int)first.size();
	for (auto j = 0ul; j < count; ++j)
	{
		std::size_t idx1 = std::uniform_int_distribution<int>{0, sz - 1}(generator);
		std::size_t idx2 = std::uniform_int_distribution<int>{0, sz - 1}(generator);
		swp(idx1, idx2, first, rest...);
	}
}

template<typename T>
template<typename C1, typename... Args>
void CT::reverse<T>::operator()(C1& first, Args&... args)
{
	CO::reverse(first);
	reverse<>{}(args...);
}

template<typename T>
template<typename C1, typename... Args>
void CT::clear<T>::operator()(C1& first, Args&... args)
{
	first.clear();
	clear<>{}(args...);
}

template<typename T>
template<typename C1, typename... Args>
bool CT::size<T>::operator()(C1& first, Args&... args)
{
	if (first.size() != count)
		return false;
	return (*this)(args...);
}

#ifdef STANDALONE

#include <iostream>
#include <list>

int main()
{
	using namespace std;
	using namespace CT;

	vector<int> vi;
	list<int>   li;

	fillup<>{}(100, vi, li);
	insert<>{}(vi, li);
	bool ok = compare<>{}(vi, li);
	cout << boolalpha << ok << endl;
}

#endif
