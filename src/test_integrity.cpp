

#include "avl_array/avl_array.hpp"
#include "avl_vector.hpp"
#include "container_operations.hpp"
#include "container_tester.hpp"
#include "inline_vector.hpp"
#include "splice_list.hpp"
#include "test_item.hpp"

#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <vector>

struct Op
{
	int op_num;
	std::vector<int> op_param;
	template<typename T>
	void Execute(T&);
	void Print(std::ostream&);
	template<typename T, typename... Args>
	void Execute(T& val, Args&&... args)
	{
		Execute(val);
		Execute(std::forward<Args>(args)...);
	}
};

enum OpIdx
{
	InsOpIdx,
	DelOpIdx,
	InsROpIdx,
	DelROpIdx,
	SortOpIdx,
	ReverseOpIdx
};

struct InsOp
{
	template<typename T>
	void Execute(std::vector<int>& param, T& cont)
	{
		int  val = param[0];
		int  pos = param[1];
		auto p   = CO::nth(cont, pos);
		cont.insert(p, val);
	};
	void Print(std::vector<int>& param, std::ostream& out)
	{
		out << "Insert " << param[0] << " at " << param[1] << std::endl;
	}
};

struct DelOp
{
	template<typename T>
	void Execute(std::vector<int>& param, T& cont)
	{
		auto p = CO::nth(cont, param[0]);
		cont.erase(p);
	};
	void Print(std::vector<int>& param, std::ostream& out) { out << "Delete node " << param[0] << std::endl; }
};

int randn(int n)
{
	return (int)rand() % n;
}

struct InsROp
{
	template<typename T>
	void Execute(std::vector<int>& param, T& cont)
	{
		auto p = CO::nth(cont, param[0]);
		cont.insert(p, param.begin() + 1, param.end());
	};
	void Print(std::vector<int>& param, std::ostream& out)
	{
		out << "Insert " << param.size() - 1 << " items at " << param[0] << std::endl;
	}
};

struct DelROp
{
	template<typename T>
	void Execute(std::vector<int>& param, T& cont)
	{
		auto p1 = CO::nth(cont, param[0]);
		auto p2 = CO::nth(cont, param[0] + param[1]);
		cont.erase(p1, p2);
	};
	void Print(std::vector<int>& param, std::ostream& out)
	{
		out << "Erase " << param[1] << " items at " << param[0] << std::endl;
	}
};

struct SortOp
{
	template<typename T>
	void Execute(std::vector<int>&, T& cont)
	{
		CO::sort(cont);
	};
	void Print(std::vector<int>&, std::ostream& out) { out << "Sort" << std::endl; }
};

struct ReverseOp
{
	template<typename T>
	void Execute(std::vector<int>&, T& cont)
	{
		CO::reverse(cont);
	};
	void Print(std::vector<int>&, std::ostream& out) { out << "Reverse" << std::endl; }
};

std::vector<Op> operlist;

template<typename T>
void Op::Execute(T& cont)
{
	// std::cout << op_num << std::endl;
	// for (auto&& x : op_param) std::cout << x << " ";
	// std::cout << std::endl;
	switch (op_num)
	{
	case InsOpIdx:     InsOp     {}.Execute(op_param, cont); break;
	case DelOpIdx:     DelOp     {}.Execute(op_param, cont); break;
	case InsROpIdx:    InsROp    {}.Execute(op_param, cont); break;
	case DelROpIdx:    DelROp    {}.Execute(op_param, cont); break;
	case SortOpIdx:    SortOp    {}.Execute(op_param, cont); break;
	case ReverseOpIdx: ReverseOp {}.Execute(op_param, cont); break;
	}
}

void Op::Print(std::ostream& out)
{
	switch (op_num)
	{
	case InsOpIdx:     InsOp     {}.Print(op_param, out); break;
	case DelOpIdx:     DelOp     {}.Print(op_param, out); break;
	case InsROpIdx:    InsROp    {}.Print(op_param, out); break;
	case DelROpIdx:    DelROp    {}.Print(op_param, out); break;
	case SortOpIdx:    SortOp    {}.Print(op_param, out); break;
	case ReverseOpIdx: ReverseOp {}.Print(op_param, out); break;
	}
}

void add_random(std::size_t& n)
{
	int i, m, j;
	if (n > 5)
		i = rand() % 6;
	else
		i = 0;
	if (n > 2500)
		i = 1;
	switch (i)
	{
	case InsOpIdx:
		operlist.push_back({InsOpIdx, {randn(1000), randn(n + 1)}});
		++n;
		break;
	case DelOpIdx:
		operlist.push_back({DelOpIdx, {randn(n)}});
		--n;
		break;
	case InsROpIdx:
	{
		m = 1 + randn(5);
		std::vector<int> param;
		param.push_back(m);
		for (int i = 0; i < m; ++i)
			param.push_back(randn(1000));
		operlist.push_back({InsROpIdx, param});
		n += m;
		break;
	}
	case DelROpIdx:
		m = 1 + randn(5);
		j = randn(n - m);
		operlist.push_back({DelROpIdx, {j, m}});
		n -= m;
		break;
	case SortOpIdx:
		operlist.push_back({SortOpIdx, {}});
		break;
	case ReverseOpIdx:
		operlist.push_back({ReverseOpIdx, {}});
		break;
	}
}

void testsuit_integrity()
{
	using namespace std;

	srand((unsigned)time(0));
	operlist.push_back({InsOpIdx, {0, 0}});
	operlist.push_back({InsOpIdx, {1, 1}});
	operlist.push_back({InsOpIdx, {2, 2}});

	std::vector<int>             vi;
	avl::vector<test_item>       avi;
	splice_list<test_item>       sli;
	inline_vector<test_item, 40> ivi;
	// mkr::avl_array<int>          aai;

#define ALL vi, avi, sli, ivi
	//, aai

	for (auto&& op : operlist)
		op.Execute(ALL);

	std::size_t i = 0, n = 3;
	bool first = true;
	std::string breakreason;
	while (true)
	{
		++i;
		if (first)
		{
			operlist.push_back({DelOpIdx, {0}});
			--n;
			first = false;
		} else {
			add_random(n);
		}

		try
		{
			operlist.back().Execute(ALL);
			if (!CT::integrity<>{}(ALL))
			{
				breakreason = "integrity";
				break;
			}
			if (!CT::size<>{n}(ALL))
			{
				breakreason = "size";
				break;
			}
			if (!CT::compare<>{}(ALL))
			{
				breakreason = "compare";
				break;
			}
			if (test_item::error())
			{
				breakreason = "ti_error";
				break;
			}
		}
		catch (...)
		{
			breakreason = "exception";
			break;
		}

		//#ifdef NDEBUG
		if ((i % 2500) == 0)
		{
			system("clear");
			// system("cls");
			std::cout << std::endl;
			std::cout << i << "\n";
			std::cout << avi.size() << "\n";
		}
		//#endif
	}

	std::cout << "\n --- error out here --- \n"
			  << "break reason : " << breakreason << "\n";

	CT::clear<>{}(ALL);
	{
		std::vector<int>             vi;
		avl::vector<test_item>       avi;
		splice_list<test_item>       sli;
		inline_vector<test_item, 40> ivi;
		// mkr::avl_array<int>          aai;

		std::size_t sz = operlist.size();
		for (i = 0; i < (sz - 1); ++i)
		{
			operlist[i].Print(std::cout);
			operlist[i].Execute(ALL);
		}
		std::cout << avi.size() << "\n";
		avi.print_tree(std::cout, false, true);
		std::cout << std::endl;
		assert(CT::compare<>{}(ALL));
		assert(CT::integrity<>{}(ALL));
		std::cout << " --- now attempting failed op --- \n";
		operlist.back().Print(std::cout);
		operlist.back().Execute(ALL);
		CT::print<>{}(std::cout, ALL);
		assert(CT::integrity<>{}(ALL));
	}

	std::cout << std::endl;
}
