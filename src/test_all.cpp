
#include "inline_vector.hpp"

#include "test_item.hpp"
#include "container_tester.hpp"

#include <iostream>

void testsuit()
{
	using namespace std;
	using namespace CT;

	vector<int> vi;
	inline_vector<int,25> ivi25;
	inline_vector<int,50> ivi250;

	fillup<>{}(10, vi, ivi25, ivi250);
	for (int i=0; i<10; ++i)
		insert<>{}(vi, ivi25, ivi250);
	for (int i=0; i<15; ++i)
		erase<>{}(vi, ivi25, ivi250);
    bool ok = compare<>{}(vi, ivi25, ivi250);
    cout << (ok?"test ok":"test failed") << endl;
	if (!ok)
	{
		cout << "vi     : ";
		for (auto i : vi) cout << i << ' ';
		cout << endl;
		cout << "ivi25  : ";
		for (auto i : ivi25) cout << i << ' ';
		cout << endl;
		cout << "ivi250 : ";
		for (auto i : ivi250) cout << i << ' ';
		cout << endl;
	}
}

int main()
{
    inline_vector<int, 4> ivi;
	for (int i=3; i<11; ++i)
		ivi.push_back(i);
	for (auto i : ivi) std::cout << i << ' ';
	std::cout << std::endl;

	{
		inline_vector<test_item, 4> ivt;
		for (int i=3; i<11; ++i)
			ivt.push_back({i});
		for (auto&& i : ivt)
			std::cout << i << ' ';
		std::cout << std::endl;
	}
	auto rep = test_item::report();
	for (auto str : rep)
		std::cout << str << std::endl;
	if (rep.empty())
		std::cout << "nothing to report" << std::endl;
	testsuit();
}


