

#include "TreeVector_allinone.hpp"

#include "container_tester.hpp"

#include <iostream>
#include <vector>
#include <list>

// _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

void testsuit()
{
	using namespace std;
	using namespace CT;
	
	TreeVector<int> tvi;
	for (int i=1; i<1'000; ++i)
	{
		tvi.push_back(i);
		if (!tvi.integrity())
		{
			std::cout << "integrity failure after " << i << std::endl;
		}
		std::cout << "\r" << i << "  ";
	}
	std::cout << std::endl;
	//std::cout << tvi << std::endl;
	while (!tvi.empty())
	{
		auto n = tvi.size();
		std::cout << "\r" << n << "  ";
		assert(n);
		auto i = rand() % n;
		auto itr = tvi.begin() + i;
		std::cout << "\r" << i << "/" << n << "   ";
		if (i==95 && n==757)
		{
			std::cout << "\nabout to do something stupid\n";
			std::cout << tvi << std::endl;
			tvi.erase(itr);
			std::cout << "\ndid the stupid\n";
			std::cout << tvi << std::endl;
		}
		else
			tvi.erase(itr);
		if (!tvi.integrity())
		{
			std::cout << "\n\n";
			assert(false);
		}
	}
}

int main()
{
	testsuit();
	//fgetc(stdin);
}


