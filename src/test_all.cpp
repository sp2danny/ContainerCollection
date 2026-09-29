
#include "avl_array/avl_array.hpp"
#include "avl_vector.hpp"
#include "inline_vector.hpp"
#include "splice_list.hpp"
#include "test_item.hpp"

#include <iostream>
#include <list>
#include <vector>
#include <print>

constexpr std::size_t REP = 3;
constexpr std::size_t SZ  = 2'000;

using namespace std::literals;

constexpr std::size_t SML = (SZ * 3) / 2;
constexpr std::size_t BIG = SZ * 3;

namespace CT
{
std::string nameof(std::vector<int>)
{
	return "std::vector<int>"s;
}
std::string nameof(avl::vector<int>)
{
	return "avl::vector<int>"s;
}
std::string nameof(std::list<int>)
{
	return "std::list<int>"s;
}
std::string nameof(splice_list<int>)
{
	return "splice_list<int>"s;
}

std::string nameof(std::vector<test_item>)
{
	return "std::vector<test_item>"s;
}
std::string nameof(avl::vector<test_item>)
{
	return "avl::vector<test_item>"s;
}
std::string nameof(std::list<test_item>)
{
	return "std::list<test_item>"s;
}
std::string nameof(splice_list<test_item>)
{
	return "splice_list<test_item>"s;
}

std::string nameof(inline_vector<test_item, SML>)
{
	return "inline_vector<test_item," + std::to_string(SML) + ">"s;
}
std::string nameof(inline_vector<test_item, BIG>)
{
	return "inline_vector<test_item," + std::to_string(BIG) + ">"s;
}

std::string nameof(mkr::avl_array<int>)
{
	return "mkr::avl_array<int>"s;
}

} // namespace CT

#include "container_tester.hpp"

void testsuit_performance()
{
	//using namespace std;
	//using namespace CT;

	std::vector<int> base;
	{
		bool ok = true;
		for (size_t i = 0; ok && (i < REP); ++i)
		{
			std::print("\r{}   ",i);
			//std::cout << "\r" << i << "   " << std::flush;
			base.clear();
			std::vector<test_item>        vti;
			std::vector<int>              vi;
			std::list<test_item>          lti;
			std::list<int>                li;
			//inline_vector<test_item, SML> ivtis;
			//inline_vector<test_item, BIG> ivtib;
			splice_list<test_item>        slti;
			splice_list<int>              sli;
			avl::vector<test_item>        avti;
			avl::vector<int>              avi;

			//#define ALL base, vti, vi, lti, li, slti, sli, avti, avi
			#define ALL base, vi, li, sli, avi

			CT::fillup<>{}(SZ, ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && CT::compare<>{}(ALL);

			
			CT::insert<>{REP}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && CT::compare<>{}(ALL);
			CT::erase<>{REP}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && CT::compare<>{}(ALL);

			/* */
			
			if (ok) for (std::size_t j = 0; ok && (j < REP); ++j)
			{
				if (ok) CT::insert<>{REP}(ALL);
				if (ok) ok = CT::integrity<>{}(ALL) && CT::compare<>{}(ALL);
				if (ok) CT::erase<>{REP}(ALL);
				if (ok) ok = CT::integrity<>{}(ALL) && CT::compare<>{}(ALL);
				if (ok) CT::nth_swap<>{REP}(ALL);
				if (ok) ok = CT::integrity<>{}(ALL) && CT::compare<>{}(ALL);
			}

			/* */

			if (ok) CT::sort<>{}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && CT::compare<>{}(ALL);

			if (ok) CT::unique<>{}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && CT::compare<>{}(ALL);

			for (size_t j = 0; ok && (j < REP); ++j)
			{
				if (ok) CT::splice_merge<>{}(ALL);
				if (ok) ok = CT::integrity<>{}(ALL) && CT::compare<>{}(ALL);
				if (ok) ok = std::is_sorted(base.begin(), base.end());
			}

			if (ok) CT::remove<>{}(test_item{SZ / 2}, ALL);
			if (ok) CT::binary_find_swap<>{}(test_item{SZ / 3}, test_item{2 * SZ / 3}, ALL);
			if (ok) CT::reverse<>{}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && CT::compare<>{}(ALL);

			if (!ok)
			{
				std::cout << "compare test failed" << std::endl;
				CT::print<>{}(std::cout, ALL);
			}
#undef ALL

			if (ok)
			{
				ok = !test_item::error();
				if (!ok)
					std::cout << "move/copy test failed" << std::endl;
			}
		}
	}

	std::cout << "\r";
	auto rep = test_item::report();
	for (auto str : rep)
		std::cout << str << std::endl;
	if (rep.empty())
		std::cout << "move/delete: nothing to report" << std::endl;

	std::cout << std::endl;

	CT::report_times /* <decltype(base)> */ (1000.0f, "ms");

}
