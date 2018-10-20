
#include "inline_vector.hpp"
#include "splice_list.hpp"

//#include "TreeVector_allinone.hpp"
//#include "TreeVector.hpp"

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
		for (int i=0; ok; ++i)
		{
			vi.clear();
			vector<test_item> vti;
			list<test_item> lti;
			inline_vector<test_item,150> ivtis;
			inline_vector<test_item,300> ivtib;
			splice_list<test_item> slti;
			//TreeVector<test_item> tvti;
			//mkr::avl_array<int> aati;
			//mkr::avl_array<int> aai;

			#define ALL vi, vti, lti, slti, ivtis, ivtib
			//#define ALL vi, vti, lti, ivtis, ivtib, slti
			//#define ALL vi, aai

			fillup<>{}(100, ALL);
			for (int i=0; i<100; ++i)
				insert<>{}(ALL);
			for (int i=0; i<150; ++i)
				erase<>{}(ALL);
			if (ok) ok = compare<>{}(ALL);
			if (ok) sort_unique<>{}(ALL);
			if (ok) ok = compare<>{}(ALL);

			if (ok) splice_merge<>{}(ALL);
			if (ok) ok = compare<>{}(ALL);
			if (ok) ok = std::is_sorted(vi.begin(), vi.end());

			if (ok) CT::remove<>{}(test_item{55}, ALL);
			if (ok) binary_find_swap<>{}(test_item{33}, test_item{66}, ALL);
			if (ok) ok = compare<>{}(ALL);

			if (!ok)
			{
				cout << "compare test failed" << endl;
				print<>{}(std::cout, ALL);
			}
			#undef ALL
			
			if ( (i>1000) && !std::is_sorted(vi.begin(), vi.end())) break;

		}
		if (ok) cout << "compare test ok" << endl;
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
	//fgetc(stdin);
}


