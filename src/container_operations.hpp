
#pragma once

#ifndef NDEBUG
#include <iostream>
#endif

#include <algorithm>

template<typename C1, typename C2, typename C3>
void merge_unique(C1& c1, C2& c2, C3& c3)
{
	if (c1.empty() && c2.empty())
		return;

	auto i1 = c1.begin();
	auto i2 = c2.begin();
	decltype(c3.begin()) last;

	auto take_1 = [&]() { last = c3.insert(c3.end(), *i1); ++i1; };
	auto take_2 = [&]() { last = c3.insert(c3.end(), *i2); ++i2; };

	if (c1.empty())
		take_2();
	else if (c2.empty())
		take_1();
	else if ((*i1) < (*i2))
		take_1();
	else
		take_2();
	while (true)
	{
		bool ae1 = (i1 == c1.end());
		bool ae2 = (i2 == c2.end());
		if (ae1 && ae2) break;
		if (ae1)
		{
			while (i2 != c2.end())
			{
				if ((*last) == (*i2))
					++i2;
				else
					take_2();
			}
			break;
		}
		else if (ae2)
		{
			while (i1 != c1.end())
			{
				if ((*last) == (*i1))
					++i1;
				else
					take_1();
			}
			break;
		}
		else
		{
			if ((*i1) < (*i2))
			{
				if ((*last) == (*i1))
					++i1;
				else
					take_1();
			}
			else
			{
				if ((*last) == (*i2))
					++i2;
				else
					take_2();
			}
		}
	}
}

namespace detail
{
	struct pick_3 {};
	struct pick_2 : pick_3 {};
	struct pick_1 : pick_2 {};

	template<typename C1>
	auto sort(pick_1, C1& c1)
		-> decltype(c1.sort(), void())
	{
		c1.sort();
	}

	template<typename C1>
	void sort(pick_2, C1& c1)
	{
		std::sort(c1.begin(), c1.end());
	}

	template<typename C1>
	auto unique(pick_1, C1& c1)
		-> decltype(c1.unique(), void())
	{
		c1.unique();
	}

	template<typename C1>
	void unique(pick_2, C1& c1)
	{
		typename C1::iterator b,e,i;
		b = c1.begin();
		e = c1.end();
		i = std::unique(b, e);
		c1.erase(i, e);
	}

	template<typename C1, typename Itm>
	auto remove(pick_1, C1& c1, const Itm& itm)
		-> decltype(c1.remove(itm), void())
	{
		c1.remove(itm);
	}

	template<typename Itm>
	auto remove(pick_2, std::vector<Itm>& c1, const Itm& itm)
	{
		auto itr = std::remove(c1.begin(), c1.end(), itm);
		c1.erase(itr, c1.end());
	}

	template<typename C1, typename Itm>
	auto remove(pick_3, C1& c1, const Itm& itm)
	{
		for (auto iter = c1.begin(); iter != c1.end();)
		{
			if (itm == *iter)
				iter = c1.erase(iter);
			else
				++iter;
		}
	}

	template<typename C1, typename Itm>
	auto binary_find(pick_1, C1& c1, const Itm& itm)
		-> std::pair<bool, typename C1::iterator>
	{
		auto iter = lower_bound(c1.begin(), c1.end(), itm);
		if (iter == c1.end())
			return {false,{}};
		if (itm == *iter)
			return {true, iter};
		else
			return {false,{}};
	}

	template<typename C1, typename Itm>
	auto binary_find(pick_2, const C1& c1, const Itm& itm)
		-> std::pair<bool, typename C1::const_iterator>
	{
		auto iter = lower_bound(c1.begin(), c1.end(), itm);
		if (iter == c1.end())
			return {false,{}};
		if (itm == *iter)
			return {true, iter};
		else
			return {false,{}};
	}
}

template<typename C1>
void sort(C1& c1)
{
	detail::sort(detail::pick_1{}, c1);
}

template<typename C1>
void unique(C1& c1)
{
	detail::unique(detail::pick_1{}, c1);
}

template<typename C1, typename Itm>
void remove(C1& c1, const Itm& itm)
{
	detail::remove(detail::pick_1{}, c1, itm);
}

template<typename C1, typename Itm>
auto binary_find(C1&& c1, const Itm& itm)
{
	detail::binary_find(detail::pick_1{}, c1, itm);
}






