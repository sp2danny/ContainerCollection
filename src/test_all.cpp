
#include "inline_vector.hpp"
#include "splice_list.hpp"
#include "avl_vector.hpp"
#include "test_item.hpp"
#include "container_tester.hpp"
//#include "avl_array.hpp"

#include <iostream>
#include <vector>
#include <list>

#define REP 15

void testsuit()
{
	/*{
		avl_vector<int> avi, rev;
		for (int i=1; i<100; ++i) avi.push_back(i);
		rev = avi;
		rev.reverse();
		auto ri = rev.crbegin();
		while (ri != rev.crend())
		{
			std::cout << *ri << ' ' << std::flush;
			++ri;
		}
		std::cout << std::endl;
	}*/

	using namespace std;
	using namespace CT;

	vector<int> vi;
	{
		bool ok = true;
		for (int i=0; ok && (i<REP); ++i)
		{
			std::cout << "\r" << i << "   " << std::flush;
			vi.clear();
			vector<test_item> vti;
			list<test_item> lti;
			inline_vector<test_item,(SZ*3)/2> ivtis;
			inline_vector<test_item,SZ*3> ivtib;
			splice_list<test_item> slti;
			avl_vector<test_item> avti;

			#define ALL vi, vti, lti, ivtis, ivtib, slti, avti

			fillup<>{}(SZ, ALL);

			//if (ok) { system("cls"); print<>{}(std::cout, ALL); }

			insert<>{SZ}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);
			erase<>{(SZ*3)/2}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);

			if (ok) for (int j=0; ok && (j<REP); ++j)
			{
				if (ok) insert<>{SZ}(ALL);
				if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);
				if (ok) erase<>{SZ}(ALL);
				if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);
				if (ok) nth_swap<>{SZ}(ALL);
				if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);
			}

			//if (true) { system("cls"); print<>{}(std::cout, ALL); }

			if (ok) CT::sort<>{}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);

			//if (true) { system("cls"); print<>{}(std::cout, ALL); }

			if (ok) CT::unique<>{}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);

			//if (true) { system("cls"); print<>{}(std::cout, ALL); }

			for (int j = 0; ok && (j < REP); ++j)
			{
				if (ok) splice_merge<>{}(ALL);
				if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);
				if (ok) ok = std::is_sorted(vi.begin(), vi.end());
			}

			if (ok) CT::remove<>{}(test_item{SZ/2}, ALL);
			if (ok) binary_find_swap<>{}(test_item{SZ/3}, test_item{2*SZ/3}, ALL);
			if (ok) CT::reverse<>{}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);

			//if (true) { system("cls"); print<>{}(std::cout, ALL); }

			if (!ok)
			{
				cout << "compare test failed" << endl;
				print<>{}(std::cout, ALL);
			}
			#undef ALL

			if (ok) {
				ok = ! test_item::error();
				if (!ok)
					cout << "move/copy test failed" << endl;
			}
		}
	}

	auto rep = test_item::report();
	for (auto str : rep)
		std::cout << str << std::endl;
	if (rep.empty())
		std::cout << "move/delete: nothing to report" << std::endl;

	cout << endl;
	report_times<decltype(vi)>();
}



