
#include "inline_vector.hpp"
#include "splice_list.hpp"

#include "TreeVector.hpp"
#include "TreeVector.cpp"

#include "test_item.hpp"
#include "container_tester.hpp"

#include "avl_tree.hpp"

#include <iostream>
#include <vector>
#include <list>

template class avl_tree<int>;

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
			TreeVector<test_item> tvti;
			//mkr::avl_array<test_item> aati;
			//mkr::avl_array<int> aai;

			#define ALL vi, vti, lti, slti, ivtis, ivtib, tvti
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

bool test_suit_avl(int N, int M, bool print)
{
	avl_tree<int> ati;
	for (int i=0; i<N; ++i)
	{
		ati.insert_sorted(rand()%M);
		if (print)
		{
			std::cout << ati.size() << "\n";
			ati.print_tree(std::cout);
			std::cout << "\n";
			assert(ati.integrity());
		}
		if (!ati.integrity()) return false;
	}
	for (int i=0; i<N; ++i)
	{
		ati.insert_sorted(rand()%M);
		auto sz = ati.size();
		auto p = ati.nth(rand()%sz);
		ati.delete_node(p);
		if (print)
		{
			std::cout << ati.size() << "\n";
			ati.print_tree(std::cout);
			std::cout << "\n";
			assert(ati.integrity());
		}
		if (!ati.integrity()) return false;
	}
	for (int i=0; i<N; ++i)
	{
		auto sz = ati.size();
		auto p = ati.nth(rand()%sz);
		ati.delete_node(p);
		if (print)
		{
			std::cout << ati.size() << "\n";
			ati.print_tree(std::cout);
			std::cout << "\n";
			assert(ati.integrity());
		}
		if (!ati.integrity()) return false;
	}
	if (print)
	{
		assert(ati.size() == 0);
	}
	return ati.size() == 0;
}

void test_suit_avl()
{
	const int M = 100'000;
	
	int N = 2;
	while (true)
	{
		srand(1);
		if (!test_suit_avl(N, M, false)) break;
		++N;
	}
	srand(1);
	test_suit_avl(N, M, true);
}

int main()
{
	//testsuit();
	test_suit_avl();
	//fgetc(stdin);
}


