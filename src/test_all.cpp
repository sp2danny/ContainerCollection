
#include "inline_vector.hpp"




#include <iostream>

int main()
{
    inline_vector<int, 4> ivi;
	for (int i=3; i<11; ++i)
		ivi.push_back(i);
	for (auto i : ivi) std::cout << i << ' ';
	std::cout << std::endl;
}


