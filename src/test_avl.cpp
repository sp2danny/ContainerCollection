
#include "test_item.hpp"
#include "avl_array/avl_array.hpp"

#include <vector>
#include <print>

void test_avl()
{
	mkr::avl_array<test_item> maati;

	maati.push_back(3);

	maati.insert(maati.end(), 4);

	maati.push_front(2);

	maati.sort(std::greater<test_item>{});

	for (auto&& x : maati)
		std::println("{}", (int)x);



}
