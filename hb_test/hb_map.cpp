
#include <string>

#include "hb_map.hpp"

template class hb_map<int, std::string>;

typedef hb_map<int, std::string> ISM;

extern "C" void test_hb_map()
{
	ISM ism;
}

