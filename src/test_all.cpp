
#include "inline_vector.hpp"
#include "splice_list.hpp"
#include "avl_vector.hpp"
#include "test_item.hpp"
#include "container_tester.hpp"

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
		for (int i=0; ok && (i<1000); ++i)
		{
			std::cout << "\r" << i << "  ";
			vi.clear();
			vector<test_item> vti;
			list<test_item> lti;
			inline_vector<test_item,150> ivtis;
			inline_vector<test_item,300> ivtib;
			splice_list<test_item> slti;
			avl_vector<test_item> avti;

			#define ALL vi, lti, ivtis, ivtib, slti, avti

			fillup<>{}(100, ALL);

			//if (ok) { system("cls"); print<>{}(std::cout, ALL); }

			for (int j=0; j<100; ++j)
				insert<>{}(ALL);
			for (int j=0; j<150; ++j)
				erase<>{}(ALL);
			if (ok) ok = compare<>{}(ALL);

			//if (true) { system("cls"); print<>{}(std::cout, ALL); }

			if (ok) CT::sort<>{}(ALL);
			if (ok) ok = compare<>{}(ALL);

			//if (true) { system("cls"); print<>{}(std::cout, ALL); }

			if (ok) CT::unique<>{}(ALL);
			if (ok) ok = compare<>{}(ALL);

			//if (true) { system("cls"); print<>{}(std::cout, ALL); }

			if (ok) splice_merge<>{}(ALL);
			if (ok) ok = compare<>{}(ALL);
			if (ok) ok = std::is_sorted(vi.begin(), vi.end());

			if (ok) CT::remove<>{}(test_item{55}, ALL);
			if (ok) binary_find_swap<>{}(test_item{33}, test_item{66}, ALL);
			if (ok) ok = compare<>{}(ALL);

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

	cout << "vi (sz:" << vi.size() << ") : ";
	for (auto i : vi) cout << i << ' ';
	cout << endl;
}

int main()
{
	testsuit();
	//test_suit_avl();
	//fgetc(stdin);
}


