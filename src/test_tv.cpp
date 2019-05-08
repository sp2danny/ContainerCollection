

#include "avl_vector.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <sstream>

//template class avl_vector<int>;

typedef avl::vector<int> ATI;
typedef std::vector<int> VI;

struct Op
{
	virtual ~Op() = default;
	virtual void Execute(ATI&, VI&, bool debug = false) = 0;
	virtual void Print(std::ostream&) = 0;
};

struct InsOp : Op
{
	InsOp(std::size_t pos, std::size_t val) : pos(pos), val(val) {}
	std::size_t pos;
	std::size_t val;
	virtual void Execute(ATI& ati, VI& vi, [[maybe_unused]] bool debug) override
	{
		auto p = ati.nth(pos);
		ati.insert(p, (int)val);
		auto pp = vi.begin() + pos;
		vi.insert(pp, (int)val);
	};
	virtual void Print(std::ostream& out) override
	{
		out << "Insert " << val << " at " << pos << std::endl;
	}
};

struct DelOp : Op
{
	DelOp(std::size_t pos) : pos(pos) {}
	std::size_t pos;
	virtual void Execute(ATI& ati, VI& vi, [[maybe_unused]] bool debug) override
	{
		auto p = ati.nth(pos);
		ati.erase(p);
		auto pp = vi.begin() + pos;
		vi.erase(pp);
	};
	virtual void Print(std::ostream& out) override
	{
		out << "Delete node " << pos << std::endl;
	}
};

std::size_t randn(std::size_t n)
{
	return (std::size_t)rand() % n;
}

struct InsROp : Op
{
	InsROp(std::size_t pos, std::size_t sz)
		: pos(pos)
	{
		for (std::size_t i = 0; i < sz; ++i)
			sli.push_back((int)randn(1000));
	}
	std::size_t pos;
	std::list<int> sli;
	virtual void Execute(ATI& ati, VI& vi, [[maybe_unused]] bool debug) override
	{
		auto p = ati.nth(pos);
		ati.insert(p, sli.begin(), sli.end());
		auto pp = vi.begin() + pos;
		vi.insert(pp, sli.begin(), sli.end());
	};
	virtual void Print(std::ostream& out) override
	{
		out << "Insert " << sli.size() << " items at " << pos << std::endl;
	}
};

struct DelROp : Op
{
	DelROp(std::size_t pos, std::size_t sz)
		: pos(pos), sz(sz)
	{
	}
	std::size_t pos, sz;
	virtual void Execute(ATI& ati, VI& vi, [[maybe_unused]] bool debug) override
	{
		auto p = ati.nth(pos);
		ati.erase(p, p+sz);
		auto pp = vi.begin() + pos;
		vi.erase(pp, pp+sz);
	};
	virtual void Print(std::ostream& out) override
	{
		out << "Erase " << sz << " items at " << pos << std::endl;
	}
};

struct SortOp : Op
{
	SortOp()
	{
	}
	virtual void Execute(ATI& ati, VI& vi, [[maybe_unused]] bool debug) override
	{
		ati.sort();
		//ati.unique();
		std::sort(vi.begin(), vi.end());
		//auto itr = std::unique(vi.begin(), vi.end());
		//vi.erase(itr, vi.end());
	};
	virtual void Print(std::ostream& out) override
	{
		out << "Sort" /* + Unique"*/ << std::endl;
	}
};

struct ReverseOp : Op
{
	ReverseOp()
	{
	}
	virtual void Execute(ATI& ati, VI& vi, [[maybe_unused]] bool debug) override
	{
		ati.reverse();
		std::reverse(vi.begin(), vi.end());
	};
	virtual void Print(std::ostream& out) override
	{
		out << "Reverse" /* + Unique"*/ << std::endl;
	}
};

typedef std::unique_ptr<Op> OpPtr;

std::vector<OpPtr> operlist;

void add_random(std::size_t& n)
{
	std::size_t i, m, j;
	if (n>5)
		i = rand()%6;
	else
		i = 0;
	if (n>500)
		i = 1;
	switch (i)
	{
	case 0:
		operlist.emplace_back(std::make_unique<InsOp>(randn(n+1), randn(1000)));
		++n;
		break;
	case 1:
		operlist.emplace_back(std::make_unique<DelOp>(randn(n)));
		--n;
		break;
	case 2:
		m = 1+randn(5);
		operlist.emplace_back(std::make_unique<InsROp>(randn(n+1), m));
		n += m;
		break;
	case 3:
		m = 1+randn(5);
		j = randn(n - m);
		operlist.emplace_back(std::make_unique<DelROp>(j, m));
		n -= m;
		break;
	case 4:
		operlist.emplace_back(std::make_unique<SortOp>());
		break;
	case 5:
		operlist.emplace_back(std::make_unique<ReverseOp>());
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

void testsuit_a()
{
	srand((unsigned)time(0));
	using namespace std;
	operlist.emplace_back(std::make_unique<InsOp>(0, 0));
	operlist.emplace_back(std::make_unique<InsOp>(1, 1));
	operlist.emplace_back(std::make_unique<InsOp>(2, 2));

	ATI ati;

	VI vi;
	for (auto&& op : operlist)
		op->Execute(ati, vi);

	std::size_t i = 0, n = 3;
	//std::stringstream ss;
	bool first = true;
	std::string breakreason;
	while (true)
	{
		++i;
		if (first)
		{
			operlist.emplace_back( std::make_unique<DelOp>(1) );
			--n;
			first = false;
		} else {
			add_random(n);
		}
		//if (_isatty(_fileno(stdout))) std::cout << operlist.size() << "    \r";

		operlist.back()->Execute(ati, vi);
		if (!ati.integrity()) { breakreason = "itegrety"; break; }
		if (ati.size() != n) { breakreason = "ati.size"; break; }
		if (vi.size() != n) { breakreason = "vi.size"; break; }
		if (!cmp(vi,ati)) { breakreason = "cmp"; break; }
		if ((i%25000)==0)
		{
			system("clear");
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
		operlist[i]->Print(std::cout);
		operlist[i]->Execute(ati, vi);
	}
	std::cout << ati.size() << "\n";
	ati.print_tree(std::cout, false, true);
	std::cout << std::endl;
	assert(cmp(vi,ati));
	std::cout << " --- now attempting failed op --- \n";
	operlist.back()->Print(std::cout);
	operlist.back()->Execute(ati, vi, true);
	ati.print_tree(std::cout, true, true);
	assert(!cmp(vi,ati));
	for (auto&& i : vi)
		std::cout << i << ' ';
	std::cout << std::endl;
}


