
#include "inline_vector.hpp"
#include "splice_list.hpp"
#include "avl_vector.hpp"
#include "test_item.hpp"
#include "container_tester.hpp"
//#include "avl_array.hpp"

#include <iostream>
#include <vector>
#include <list>

void testsuit()
{
	using namespace std;
	using namespace CT;

	vector<int> vi;
	{
		bool ok = true;
		for (int i=0; ok && (i<5); ++i)
		{
			std::cout << "\r" << i << "   " << std::flush;
			vi.clear();
			vector<test_item> vti;
			list<test_item> lti;
			inline_vector<test_item,15000> ivtis;
			inline_vector<test_item,30000> ivtib;
			splice_list<test_item> slti;
			avl_vector<test_item> avti;

			#define ALL vi, vti, lti, ivtis, ivtib, slti, avti

			fillup<>{}(10000, ALL);

			//if (ok) { system("cls"); print<>{}(std::cout, ALL); }

			insert<>{10000}(ALL);
			erase<>{15000}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);
			
			for (int j=0; j<5; ++j)
			{

				insert<>{10000}(ALL);
				erase<>{10000}(ALL);
				if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);
			}

			//if (true) { system("cls"); print<>{}(std::cout, ALL); }

			if (ok) CT::sort<>{}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);

			//if (true) { system("cls"); print<>{}(std::cout, ALL); }

			if (ok) CT::unique<>{}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);

			//if (true) { system("cls"); print<>{}(std::cout, ALL); }

			if (ok) splice_merge<>{}(ALL);
			if (ok) ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);
			if (ok) ok = std::is_sorted(vi.begin(), vi.end());

			if (ok) CT::remove<>{}(test_item{ 55 }, ALL);
			if (ok) binary_find_swap<>{}(test_item{ 33 }, test_item{ 66 }, ALL);
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

	//cout << "vi (sz:" << vi.size() << ") : ";
	//for (auto i : vi) cout << i << ' ';
	cout << endl;
	report_times<decltype(vi)>();
}

void test_suit_avl()
{
	avl_vector<int> avi;
	int i = 1, j = 1;
	while (true)
	{
		avi.push_back(i);
		++i; ++j;
		if (j==137913)
		{
			std::cerr << i << "\r";
			j = 0;
		}
		if (avi.size()==avi.max_size())
			break;
	}
	avi.print_tree(std::cout);
}



