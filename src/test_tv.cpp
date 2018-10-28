

#include "avl_tree.hpp"

//#include "hb_tree.hpp"

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

template class avl_tree<int>;

typedef avl_tree<int> ATI;
typedef std::vector<int> VI;

struct Op
{
	virtual ~Op() = default;
	virtual void Execute(ATI&, VI&, bool debug = false) = 0;
	virtual void Print(std::ostream&) = 0;
};

struct InsOp : Op
{
	InsOp(int pos, int val) : pos(pos), val(val) {}
	int pos;
	int val;
	virtual void Execute(ATI& ati, VI& vi, bool debug) override
	{
		auto p = ati.nth(pos);
		ati.insert(p, val);
		auto pp = vi.begin() + pos;
		vi.insert(pp, val);
	};
	virtual void Print(std::ostream& out) override
	{
		out << "Insert " << val << " at " << pos << std::endl;
	}
};

struct DelOp : Op
{
	DelOp(int pos) : pos(pos) {}
	int pos;
	virtual void Execute(ATI& ati, VI& vi, bool debug) override
	{
		auto p = ati.nth(pos);
		ati.delete_node(p);
		auto pp = vi.begin() + pos;
		vi.erase(pp);
	};
	virtual void Print(std::ostream& out) override
	{
		out << "Delete node " << pos << std::endl;
	}
};

typedef std::unique_ptr<Op> OpPtr;

std::vector<OpPtr> operlist;

void add_random(std::size_t& n)
{
	int i;
	if (n)
		i = rand()%2;
	else
		i = 0;
	if (n>80)
		i = 1;
	switch (i)
	{
	case 0:
		operlist.emplace_back( std::make_unique<InsOp>(rand()%(n+1),rand()%1000) );
		++n;
		break;
	case 1:
		operlist.emplace_back( std::make_unique<DelOp>(rand()%(n)) );
		--n;
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
			if (vi[j] != ati.nth(j)->item)
				break;
		if (j<n) break;
		if ((i%25000)==0)
		{
			system("cls");
			std::cout << i << "\n";
			std::cout << ati.size() << "\n";
			ati.print_tree(std::cout);
			std::cout << std::endl;
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


