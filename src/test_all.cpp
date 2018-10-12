
#include "inline_vector.hpp"


#include "test_item.hpp"

#include <iostream>

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

}


