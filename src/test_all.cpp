
#include "inline_vector.hpp"




#include <iostream>

int main()
{
    inline_vector<int, 4> ivi;
	ivi.push_back(3);
	for (auto i : ivi) std::cout << i << ' ';
	std::cout << std::endl;
}


