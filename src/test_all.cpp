
#include "inline_vector.hpp"
#include "splice_list.hpp"
#include "TreeVector.hpp"

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
		for (int i=0; ok && (i<100); ++i)
		{
			vi.clear();
			vector<test_item> vti;
			list<test_item> lti;
			inline_vector<test_item,150> ivtis;
			inline_vector<test_item,300> ivtib;
			splice_list<test_item> slti;
			//TreeVector<test_item> tvi;

			#define ALL vi, vti, lti, ivtis, ivtib, slti

			fillup<>{}(100, ALL);
			for (int i=0; i<100; ++i)
				insert<>{}(ALL);
			for (int i=0; i<150; ++i)
				erase<>{}(ALL);
			if (ok) ok = compare<>{}(ALL);
			if (ok) sort_unique<>{}(ALL);
			if (ok) ok = compare<>{}(ALL);

			if (!ok)
			{
				print<>{}(std::cout, ALL);
			}
			#undef ALL

		}
		cout << (ok?"compare test ok":"compare test failed") << endl;
	}

	auto rep = test_item::report();
	for (auto str : rep)
		std::cout << str << std::endl;
	if (rep.empty())
		std::cout << "move/delete: nothing to report" << std::endl;

	cout << "vi     : ";
	for (auto i : vi) cout << i << ' ';
	cout << endl;

}

int main()
{
	testsuit();
}


