

#include "avl_vector.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <sstream>

//template class hb_tree<int>;

struct immobile
{
	immobile() = delete;
	immobile(int, int) {}
	immobile(const immobile&) = delete;
	immobile(immobile&&) = delete;
	immobile& operator=(const immobile&) = delete;
	immobile& operator=(immobile&&) = delete;
	~immobile() = default;
};

//template class hb_tree<immobile>;

template class avl_vector<int>;

typedef avl_vector<int> ATI;
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
	virtual void Execute(ATI& ati, VI& vi, bool debug) override
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
	virtual void Execute(ATI& ati, VI& vi, bool debug) override
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
	virtual void Execute(ATI& ati, VI& vi, bool debug) override
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


typedef std::unique_ptr<Op> OpPtr;

std::vector<OpPtr> operlist;

void add_random(std::size_t& n)
{
	std::size_t i, m;
	if (n)
		i = rand()%3;
	else
		i = 0;
	if (n>70)
		i = 1;
	switch (i)
	{
	case 0:
		operlist.emplace_back( std::make_unique<InsOp>(randn(n+1), randn(1000)) );
		++n;
		break;
	case 1:
		operlist.emplace_back( std::make_unique<DelOp>(randn(n)) );
		--n;
		break;
	case 2:
		m = 1+randn(5);
		operlist.emplace_back(std::make_unique<InsROp>(randn(n+1), m));
		n += m;
		break;
	}
}

//#include <unistd.h>
#include <io.h>

void testsuit()
{
	srand((unsigned)time(0));
	using namespace std;
	operlist.emplace_back( std::make_unique<InsOp>(0,0) );
	operlist.emplace_back( std::make_unique<InsOp>(1,1) );
	operlist.emplace_back( std::make_unique<InsOp>(2,2) );

	ATI ati;
	VI vi;
	for (auto&& op : operlist)
		op->Execute(ati, vi);

	std::size_t j, i = 0, n = 3;
	//std::stringstream ss;
	bool first = true;
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
		if (!ati.integrity())
			break;
		if (ati.size() != n) break;
		if (vi.size() != n) break;
		for (j=0; j<n; ++j)
			if (vi[j] != ati[j])
				break;
		if (j<n) break;
		if ((i%25000)==0)
		{
			system("cls");
			for (auto&& x : ati) std::cout << x << " ";
			std::cout << std::endl;
			std::cout << i << "\n";
			std::cout << ati.size() << "\n";
			ati.print_tree(std::cout);
			std::cout << std::endl;
			ATI ati2{ati};
			ati2.sort();
			for (auto&& x : ati2) std::cout << x << " ";
			assert(ati2.integrity());
			assert(ati2.is_sorted());
		}
	}

	ati.clear(); vi.clear();
	std::size_t sz = operlist.size();
	for (i=0; i<(sz-1); ++i)
	{
		operlist[i]->Print(std::cout);
		operlist[i]->Execute(ati, vi);
	}
	std::cout << ati.size() << "\n";
	ati.print_tree(std::cout);
	std::cout << std::endl;
	operlist.back()->Print(std::cout);
	operlist.back()->Execute(ati, vi, true);
	ati.print_tree(std::cout);
}

int main()
{
	testsuit();
	//fgetc(stdin);
}


