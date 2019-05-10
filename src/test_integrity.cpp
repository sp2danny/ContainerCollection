

#include "avl_vector.hpp"
#include "splice_list.hpp"
#include "container_operations.hpp"
#include "inline_vector.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <sstream>

struct Op
{
	int op_num;
	std::vector<int> op_param;
	template<typename T>
	void Execute(T&);
	void Print(std::ostream&);
	template<typename T,typename... Args>
	void Execute(T& val, Args&&... args)
	{
		Execute(val);
		Execute(std::forward<Args>(args)...);
	}
};

enum OpIdx { InsOpIdx, DelOpIdx, InsROpIdx, DelROpIdx, SortOpIdx, ReverseOpIdx };

struct InsOp
{
	template<typename T>
	void Execute(std::vector<int>& param, T& cont)
	{
		int val = param[0];
		int pos = param[1];
		auto p = CO::nth(cont, pos);
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
	void Print(std::vector<int>& param, std::ostream& out)
	{
		out << "Delete node " << param[0] << std::endl;
	}
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
		cont.insert(p, param.begin()+1, param.end());
	};
	void Print(std::vector<int>& param, std::ostream& out)
	{
		out << "Insert " << param.size()-1 << " items at " << param[0] << std::endl;
	}
};

struct DelROp
{
	template<typename T>
	void Execute(std::vector<int>& param, T& cont)
	{
		auto p1 = CO::nth(cont, param[0]);
		auto p2 = CO::nth(cont, param[0]+param[1]);
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
	void Print(std::vector<int>&, std::ostream& out)
	{
		out << "Sort" << std::endl;
	}
};

struct ReverseOp
{
	template<typename T>
	void Execute(std::vector<int>&, T& cont)
	{
		CO::reverse(cont);
	};
	void Print(std::vector<int>&, std::ostream& out)
	{
		out << "Reverse" << std::endl;
	}
};

std::vector<Op> operlist;

template<typename T>
void Op::Execute(T& cont)
{
	//std::cout << op_num << std::endl;
	//for (auto&& x : op_param) std::cout << x << " ";
	//std::cout << std::endl;
	switch (op_num)
	{
		case InsOpIdx      : InsOp     {}.Execute(op_param, cont); break;
		case DelOpIdx      : DelOp     {}.Execute(op_param, cont); break;
		case InsROpIdx     : InsROp    {}.Execute(op_param, cont); break;
		case DelROpIdx     : DelROp    {}.Execute(op_param, cont); break;
		case SortOpIdx     : SortOp    {}.Execute(op_param, cont); break;
		case ReverseOpIdx  : ReverseOp {}.Execute(op_param, cont); break;
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
	if (n>5)
		i = rand()%6;
	else
		i = 0;
	if (n>500)
		i = 1;
	switch (i)
	{
	case InsOpIdx:
		operlist.push_back({InsOpIdx, {randn(1000), randn(n+1)}});
		++n;
		break;
	case DelOpIdx:
		operlist.push_back({DelOpIdx, {randn(n)}});
		--n;
		break;
	case InsROpIdx:
	{
		m = 1+randn(5);
		std::vector<int> param;
		param.push_back(m);
		for (int i=0; i<m; ++i)
			param.push_back(randn(1000));
		operlist.push_back({InsROpIdx, param});
		n += m;
		break;
	}
	case DelROpIdx:
		m = 1+randn(5);
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

//#include <unistd.h>
//#include <io.h>

template<typename C1, typename C2>
bool cmp(const C1& c1, const C2& c2)
{
	auto i1 = std::begin(c1);
	auto i2 = std::begin(c2);
	while (true)
	{
		bool ate1 = (i1 == std::end(c1));
		bool ate2 = (i2 == std::end(c2));
		if (ate1 && ate2) return true;
		if (ate1 || ate2) return false;
		if (*i1  !=  *i2) return false;
		++i1; ++i2;
	}
}

template<typename C1, typename C2, typename... Args>
bool cmp(const C1& c1, const C2& c2, const Args&... args)
{
	auto i1 = std::begin(c1);
	auto i2 = std::begin(c2);
	while (true)
	{
		bool ate1 = (i1 == std::end(c1));
		bool ate2 = (i2 == std::end(c2));
		if (ate1 && ate2) return true;
		if (ate1 || ate2) return false;
		if (*i1  !=  *i2) return false;
		++i1; ++i2;
	}
	cmp(c1, args...);
}

void testsuit_integrity()
{
	srand((unsigned)time(0));
	using namespace std;
	operlist.push_back({InsOpIdx, {0, 0}});
	operlist.push_back({InsOpIdx, {1, 1}});
	operlist.push_back({InsOpIdx, {2, 2}});

	avl::vector<int>      ati;
	std::vector<int>      vi;
	splice_list<int>      sli;
	inline_vector<int,40> ivi;

	#define ALL ati, vi, sli, ivi

	for (auto&& op : operlist)
		op.Execute(ALL);

	std::size_t i = 0, n = 3;
	//std::stringstream ss;
	bool first = true;
	std::string breakreason;
	while (true)
	{
		++i;
		if (first)
		{
			operlist.push_back({DelOpIdx,{0}});
			--n;
			first = false;
		} else {
			add_random(n);
		}
		//if (_isatty(_fileno(stdout))) std::cout << operlist.size() << "    \r";

		operlist.back().Execute(ALL);
		if (!ati.integrity()) { breakreason = "itegrety"; break; }
		if (ati.size() != n) { breakreason = "ati.size"; break; }
		if (vi.size() != n) { breakreason = "vi.size"; break; }
		if (sli.size() != n) { breakreason = "sli.size"; break; }
		if (!cmp(vi,ati)) { breakreason = "cmp.ati"; break; }
		if (!cmp(vi,sli)) { breakreason = "cmp.sli"; break; }
		if ((i%2500)==0)
		{
			//system("clear");
			system("cls");
			//for (auto&& x : ati) std::cout << x << " ";
			std::cout << std::endl;
			std::cout << i << "\n";
			std::cout << ati.size() << "\n";
			//ati.print_tree(std::cout, false, true);
			std::cout << std::endl;
			//ATI ati2{ati};
			//ati2.sort();
			//for (auto&& x : ati2) std::cout << x << " ";
			//assert(ati2.integrity());
			//assert(ati2.is_sorted());
		}
	}

	std::cout << "\n --- error out here --- \n" << "break reason : " << breakreason << "\n";

	ati.clear(); vi.clear();
	std::size_t sz = operlist.size();
	for (i=0; i<(sz-1); ++i)
	{
		operlist[i].Print(std::cout);
		operlist[i].Execute(ALL);
	}
	std::cout << ati.size() << "\n";
	ati.print_tree(std::cout, false, true);
	std::cout << std::endl;
	assert(cmp(ALL));
	std::cout << " --- now attempting failed op --- \n";
	operlist.back().Print(std::cout);
	operlist.back().Execute(ALL);
	ati.print_tree(std::cout, true, true);
	assert(!cmp(ALL));
	for (auto&& i : vi)
		std::cout << i << ' ';
	std::cout << std::endl;
}


