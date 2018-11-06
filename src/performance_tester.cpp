
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

DataVec vecData;

void insert_test(std::size_t sz)
{
	avl_vector<int> avi;
	std::vector<int> vi;

	CT::fillup<>{}(sz, vi, avi);

	CT::clear_times();
	auto insert = CT::insert<>{sz};
	insert(vi, avi);

	Data data;
	data.time = CT::time_data[CT::nameof(avi)]["insert_nth"];
	data.size = sz;
	vecData.push_back(data);

	std::cout << "testng " << sz << std::endl;
	CT::report_times<decltype(vi)>();
	std::cout << std::endl;
}

void testsuit()
{
	for (int i=0; i<100; ++i)
	{
		insert_test(100);
		insert_test(350);
		insert_test(1000);
		insert_test(3500);
		insert_test(10000);
		insert_test(35000);
		insert_test(100000);
	}

	for (auto&& x : vecData)
	{
		std::cout << x.time << " : " << x.size << std::endl;
	}
	
	extern void fitting();
	fitting();
}

struct Curve
{
	double base    = 0.0;
	double linear  = 0.0;
	double power   = 1.5; //5;
	double pfactor = 9e-9; //9.6e-9;
};

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
	double err = (data-pred);
	return std::abs(err*err);
}

double sum_square_error(const Curve& crv, const DataVec& dvec)
{
	double sum = 0.0;
	for (auto&& x : dvec)
	{
		sum += square_error(crv, x.size, x.time);
	}
	return sum;
}

void nudge_base(Curve& crv, double amount)
{
	crv.base += amount;
}

void nudge_linear(Curve& crv, double amount)
{
	crv.linear += amount;
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
	crv.pfactor += amount;
}

void continuos_nudge(Curve& crv, double amount)
{
	(void)crv;
	(void)amount;

	double sse, bsf = sum_square_error(crv, vecData);
	Curve oth = crv;

	while (true)
	{
		bool better = false;

		nudge_base(oth, amount);
		sse = sum_square_error(oth, vecData);
		if (sse < bsf)
		{
			bsf = sse;
			crv = oth;
			better = true;
		} else
			oth = crv;
			
		nudge_base(oth, -amount);
		sse = sum_square_error(oth, vecData);
		if (sse < bsf)
		{
			bsf = sse;
			crv = oth;
			better = true;
		} else
			oth = crv;

		nudge_linear(oth, amount);
		sse = sum_square_error(oth, vecData);
		if (sse < bsf)
		{
			bsf = sse;
			crv = oth;
			better = true;
		} else
			oth = crv;
			
		nudge_linear(oth, -amount);
		sse = sum_square_error(oth, vecData);
		if (sse < bsf)
		{
			bsf = sse;
			crv = oth;
			better = true;
		} else
			oth = crv;

		nudge_power(oth, amount);
		sse = sum_square_error(oth, vecData);
		if (sse < bsf)
		{
			bsf = sse;
			crv = oth;
			better = true;
		} else
			oth = crv;
			
		nudge_power(oth, -amount);
		sse = sum_square_error(oth, vecData);
		if (sse < bsf)
		{
			bsf = sse;
			crv = oth;
			better = true;
		} else
			oth = crv;

		nudge_pfactor(oth, amount);
		sse = sum_square_error(oth, vecData);
		if (sse < bsf)
		{
			bsf = sse;
			crv = oth;
			better = true;
		} else
			oth = crv;

		nudge_pfactor(oth, -amount);
		sse = sum_square_error(oth, vecData);
		if (sse < bsf)
		{
			bsf = sse;
			crv = oth;
			better = true;
		} else
			oth = crv;

		if (!better) break;
	}
}

void fitting()
{
	Curve crv;
	double amount = 0.1;
	for (int i=0; i<8; ++i)
	{
		continuos_nudge(crv, amount);
		amount *= 0.1;
	}
	std::cout << std::endl << "curve fitting" << std::endl;
	std::cout << "base    " << crv.base    << std::endl; 
	std::cout << "linear  " << crv.linear  << std::endl;
	std::cout << "power   " << crv.power   << std::endl;
	std::cout << "pfactor " << crv.pfactor << std::endl;
	std::cout << std::endl << "SSE : " << sum_square_error(crv, vecData) << std::endl;
}





