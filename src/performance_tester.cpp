
#include "inline_vector.hpp"
#include "splice_list.hpp"
#include "avl_vector.hpp"
#include "container_tester.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <cmath>

struct Data
{
	double time;
	std::size_t size;
};

typedef std::vector<Data> DataVec;

DataVec insertData;
DataVec eraseData;
DataVec splicemergeData;

void all_test(std::size_t sz, bool last = false)
{
	std::vector<int> subject;
	//avl_vector<int> subject;
	//std::list<int> subject;

	CT::fillup<>{}(sz, subject);

	CT::clear_times();
	auto insert = CT::insert<>{sz};
	insert(subject);

	Data data;
	data.time = CT::time_data[CT::nameof(subject)]["insert_nth"];
	data.size = sz;
	insertData.push_back(data);
	
	CT::clear_times();
	auto erase = CT::erase<>{sz};
	erase(subject);
	
	data.time = CT::time_data[CT::nameof(subject)]["erase_nth"];
	data.size = sz;
	eraseData.push_back(data);
	
	sort(subject);
	
	CT::clear_times();
	auto splice_merge = CT::splice_merge<>{};
	splice_merge(subject);
	
	data.time = CT::time_data[CT::nameof(subject)]["splice_merge"];
	data.size = sz;
	splicemergeData.push_back(data);


	if (last) std::cout << "done testing " << CT::nameof(subject) << std::endl;
}

extern void fitting(const DataVec&, std::string);

void testsuit()
{
	for (int i=0; i<15; ++i)
	{
		std::cout << i << "\r" << std::flush;
		all_test(100+i);
		all_test(350+i*3);
		all_test(1000+i*10);
		all_test(3500+i*35);
		all_test(10000+i*100);
		all_test(35000+i*350);
	}
	all_test(50000, true);
	
	fitting(insertData, "insert_nth");
	fitting(eraseData, "erase_nth");
	fitting(splicemergeData, "splice_merge");
}

struct Curve
{
	double base    = 0.0;
	double linear  = 0.0;
	double power   = 1.5; //5;
	double pfactor = 1e-8; //9.6e-9;
};

const Curve baseline { 1.0, 1.0, 1.0, 1e-8 };

double executeCurve(const Curve& crv, double inp)
{
	double outp = crv.base;
	outp += crv.linear * inp;
	outp += crv.pfactor * std::pow(inp, crv.power);
	return outp;
}

double square_error(const Curve& crv, double inp, double data)
{
	double pred = executeCurve(crv, inp);
	double err = (data-pred) / data;
	return std::abs(err*err);
}

double sum_square_error(const Curve& crv, const DataVec& dvec)
{
	double sum = 0.0;
	for (auto&& x : dvec)
	{
		sum += square_error(crv, x.size, x.time);
	}
	return sum / dvec.size();
}

void nudge_base(Curve& crv, double amount)
{
	crv.base += amount * baseline.base;
}

void nudge_linear(Curve& crv, double amount)
{
	crv.linear += amount * baseline.linear;
}

void nudge_power(Curve& crv, double amount)
{
	if (amount > 0)
		crv.power *= (1+amount);
	else
		crv.power /= (1-amount);
}

void nudge_pfactor(Curve& crv, double amount)
{
	crv.pfactor += amount * baseline.pfactor;
}

struct Nudge
{
	signed char b,l,p,f;
};


auto mk_arr()
{
	std::vector<Nudge> arr;
	std::vector<signed char> dir = { 0, +1, -1 };
	for (auto b : dir)
		for (auto l : dir)
			for (auto p : dir)
				for (auto f : dir)
					arr.push_back({b,l,p,f});
	return arr;
}

const std::vector<Nudge> arr = mk_arr();

void execute_nudge(Curve& crv, int index, double amount)
{
	nudge_base    (crv, amount * arr[index].b );
	nudge_linear  (crv, amount * arr[index].l );
	nudge_power   (crv, amount * arr[index].p );
	nudge_pfactor (crv, amount * arr[index].f );
}

int best_nudge(const Curve& crv, const DataVec& dvec, double amount)
{
	int bsf = 0;
	double sse, minerr = sum_square_error(crv, dvec);
	int n = (int)arr.size();
	for (int idx = 0; idx < n; ++idx)
	{
		Curve oth = crv;
		execute_nudge(oth, idx, amount);
		sse = sum_square_error(oth, dvec);
		if (sse < minerr)
		{
			minerr = sse;
			bsf = idx;
		}
	}
	return bsf;
}

void continuos_nudge(Curve& crv, const DataVec& dvec, double amount)
{
	int idx = best_nudge(crv, dvec, amount);
	if (!idx) return;
	
	execute_nudge(crv, idx, amount);
	double sse, minerr = sum_square_error(crv, dvec);

	while (true)
	{
		Curve oth = crv;
		execute_nudge(oth, idx, amount);
		sse = sum_square_error(oth, dvec);
		if (sse >= minerr) break;
		minerr = sse;
		crv = oth;
	}
}

void fitting(const DataVec& dvec, std::string name)
{
	Curve crv;
	double amount = 0.1;
	for (int i=0; i<35; ++i)
	{
		continuos_nudge(crv, dvec, amount);
		amount *= 0.5;
	}
	std::cout << std::endl << "curve fitting for " << name << std::endl;
	std::cout << "base    " << crv.base    << std::endl; 
	std::cout << "linear  " << crv.linear  << std::endl;
	std::cout << "power   " << crv.power   << std::endl;
	std::cout << "pfactor " << crv.pfactor << std::endl;
	std::cout << std::endl << "SSE : " << sum_square_error(crv, dvec) << std::endl;
}





