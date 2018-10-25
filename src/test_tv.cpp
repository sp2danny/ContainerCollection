

#include "avl_tree.hpp"

#include "hb_tree.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <sstream>

template class hb_tree<int>;

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

template class hb_tree<immobile>;

template class avl_tree<int>;

typedef avl_tree<int> ATI;

struct Op
{
	virtual ~Op() = default;
	virtual void Execute(ATI&) = 0;
	virtual void Print(std::ostream&) = 0;
};

struct InsOp : Op
{
	InsOp(int pos, int val) : pos(pos), val(val) {}
	int pos;
	int val;
	virtual void Execute(ATI& ati) override
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
	virtual void Execute(ATI& ati) override
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

void add_random(int& n)
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

void testsuit()
{
	using namespace std;
	operlist.emplace_back( std::make_unique<InsOp>(0,0) );
	operlist.emplace_back( std::make_unique<InsOp>(1,1) );
	operlist.emplace_back( std::make_unique<InsOp>(2,2) );

	ATI ati;
	for (auto&& op : operlist)
		op->Execute(ati);

	int n = 3;
	std::stringstream ss;
	bool first = true;
	while (true)
	{
		if (first)
		{
			operlist.emplace_back( std::make_unique<DelOp>(1) );
			--n;
			first = false;
		} else {
			add_random(n);
		}
		//if (isatty(fileno(stdout))) std::cout << operlist.size() << "    \r";
		ss.str("");
		ss << ati.size() << "\n";
		ati.print_tree(ss);
		operlist.back()->Execute(ati);
		if (!ati.integrity())
			break;
	}
	std::cout << "\n\n";
	
	std::size_t i, sz = operlist.size();
	for (i=0; i<(sz-1); ++i)
	{
		operlist[i]->Print(std::cout);
	}
	std::cout << ss.str() << std::endl;
	operlist.back()->Print(std::cout);
	ati.print_tree(std::cout);
}

int main()
{
	testsuit();
	//fgetc(stdin);
}


