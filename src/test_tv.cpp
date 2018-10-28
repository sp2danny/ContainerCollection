

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

struct Op
{
	virtual ~Op() = default;
	virtual void Execute(ATI&, bool debug = false) = 0;
	virtual void Print(std::ostream&) = 0;
};

struct InsOp : Op
{
	InsOp(int pos, int val) : pos(pos), val(val) {}
	int pos;
	int val;
	virtual void Execute(ATI& ati, bool debug) override
	{
		auto p = ati.nth(pos);
		ati.insert(p, val);
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
	virtual void Execute(ATI& ati, bool debug) override
	{
		auto p = ati.nth(pos);
		ati.delete_node(p);
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
	using namespace std;
	operlist.emplace_back( std::make_unique<InsOp>(0,0) );
	operlist.emplace_back( std::make_unique<InsOp>(1,1) );
	operlist.emplace_back( std::make_unique<InsOp>(2,2) );

	ATI ati;
	for (auto&& op : operlist)
		op->Execute(ati);

	std::size_t i = 0, n = 3;
	std::stringstream ss;
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
		ss.str("");
		ss << ati.size() << "\n";
		ati.print_tree(ss);
		operlist.back()->Execute(ati);
		if (!ati.integrity())
			break;
		if ((i%100)==0)
		{
			system("cls");
			std::cout << ss.str() << std::endl;
		}
	}

	ati.clear();
	std::size_t sz = operlist.size();
	for (i=0; i<(sz-1); ++i)
	{
		operlist[i]->Print(std::cout);
		operlist[i]->Execute(ati);
	}
	std::cout << ss.str() << std::endl;
	operlist.back()->Print(std::cout);
	operlist.back()->Execute(ati, true);
	ati.print_tree(std::cout);
}

int main()
{
	testsuit();
	//fgetc(stdin);
}


