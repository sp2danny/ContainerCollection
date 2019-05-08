
#include "inline_vector.hpp"
#include "splice_list.hpp"
#include "avl_vector.hpp"
#include "container_tester.hpp"

// #include "asyn_kb.h"
#include "graph.h"

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <cmath>
#include <fstream>

struct Data
{
	std::size_t size;
	double insert_time;
	double splice_time;
	double sort_time;
};

typedef std::vector<Data> DataVec;

DataVec vectorData, treeData, listData;

void all_test(std::size_t sz, bool last = false)
{
	CT::clear_times();

	std::vector<int> vi;
	avl::vector<int> ti;
	std::list<int>   li;

	#define ALL vi, ti, li

	CT::fillup<>{}(sz, vi, ti, li);

	CT::insert<>{sz}(ALL);
	CT::erase<>{sz}(ALL);
	CT::sort<>{}(ALL);
	CT::splice_merge<>{}(ALL);
	
	#undef ALL

	auto mkdata = [sz](auto cont) -> Data
	{
		Data data;
		data.size = sz;
		auto name = CT::nameof(cont);
		data.insert_time =  CT::time_data[name]["insert_nth"];
		data.insert_time += CT::time_data[name]["erase_nth"];
		data.splice_time =  CT::time_data[name]["splice_merge"];
		data.sort_time   =  CT::time_data[name]["sort"];
		return data;
	};

	vectorData .push_back(mkdata(vi));
	treeData   .push_back(mkdata(ti));
	listData   .push_back(mkdata(li));

	if (last) CT::report_times<>();

	if (last) std::cout << "done testing " << std::endl;
}

extern void fitting(const DataVec&, std::string);

void testsuit()
{
	avl::vector<int> avi;
	for (int i=1; i<=22; ++i) avi.push_back(i);
	avi.print_tree(std::cout, false, true);

	//AsynKB::Start();
	//all_test(50000, false);
	for (int j=0; j<60; ++j)
	{
		int i = j/3;
		#ifndef NDEBUG
		if (j%3) continue;
		#endif
		std::cout << i << "." << (j%3) << "\r" << std::flush;
		all_test(  100+i*10   + j%3);
		all_test(  350+i*35   + j%3);
		all_test( 1000+i*100  + j%3);
		all_test( 3500+i*350  + j%3);
		all_test(10000+i*1000 + j%3);

		#ifdef NDEBUG
		all_test(35000+i*3500 + j%3);
		all_test(3610 +i*350  + j%3);
		all_test(3720 +i*350  + j%3);
		all_test(10330+i*1000 + j%3);
		all_test(10660+i*1000 + j%3);
		all_test(36100+i*3500 + j%3);
		all_test(37200+i*3500 + j%3);
		#endif
	}
	std::cout << 20 << "\r" << std::flush;
	all_test(10000, true);

	auto mkimg = [](const DataVec& dv, std::string name) -> void
	{
		MultiPlot mp;
		for (auto&& itm : dv)
		{
			mp.AddPoint({255,127,127}, (double)itm.size, itm.insert_time);
			mp.AddPoint({127,255,127}, (double)itm.size, itm.splice_time);
			mp.AddPoint({127,127,255}, (double)itm.size, itm.sort_time);
		}
		Image img = mp.generate(1024,768);
		img.Save(name);
	};

	mkimg(vectorData, "VectorData.bmp");
	mkimg(treeData,   "TreeData.bmp");
	mkimg(listData,   "ListData.bmp");

	auto mkimg2 = [](const DataVec& vec, const DataVec& tree, const DataVec& lst) -> void
	{
		MultiPlot mp;
		for (auto&& itm : vec)
			mp.AddPoint({255,127,127}, (double)itm.size, itm.insert_time + itm.splice_time + itm.sort_time);
		for (auto&& itm : tree)
			mp.AddPoint({127,255,127}, (double)itm.size, itm.insert_time + itm.splice_time + itm.sort_time);
		for (auto&& itm : lst)
			mp.AddPoint({127,127,255}, (double)itm.size, itm.insert_time + itm.splice_time + itm.sort_time);
		Image img = mp.generate(1024,768);
		img.Save("all.bmp");
	};

	mkimg2(vectorData, treeData, listData);

	//fitting(insertData, "insert_nth");
	//fitting(eraseData, "erase_nth");
	//fitting(splicemergeData, "splice_merge");

	/*
	std::cout << "\n";
	for (auto&& post : splicemergeData)
	{
		auto [x,y] = post;
		std::cout << x << "\t" << y << "\n";
	}
	std::cout << std::endl;
	*/
}

/*
struct Curve
{
	double base    = 0.0;
	double linear  = 0.0;
	double power   = 1.5;
	double pfactor = 5e-9;
};

Curve baseline { 1e-4, 1e-4, 0.1, 1e-9 };

void initBase()
{
	baseline = { 1e-4, 1e-4, 0.1, 1e-9 };
}

void updB(double val, double& base)
{
	double l = std::log10(val);
	if (l < -12) l = -12;
	if (l >  -2) l =  -2;
	base = std::pow(10, l-1);
}

void updateBase(const Curve& crv)
{
	updB(crv.base,    baseline.base);
	updB(crv.linear,  baseline.linear);
	//updB(crv.power,   baseline.power);
	updB(crv.pfactor, baseline.pfactor);
}

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
	for (auto&& item : dvec)
	{
		auto [x, y] = item;
		sum += square_error(crv, x, y);
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
	short b,l,p,f;
};

auto mk_arr()
{
	std::vector<Nudge> arr;
	std::vector<short> dir = { 0, +1, -1, +3, -3, +10, -10, +35, -35, +100, -100 };
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
	nudge_base    (crv, amount * arr[index].b);
	nudge_linear  (crv, amount * arr[index].l);
	nudge_power   (crv, amount * arr[index].p);
	nudge_pfactor (crv, amount * arr[index].f);
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

bool continuos_nudge(Curve& crv, const DataVec& dvec, double amount, int& count, int ii)
{
	double sse = sum_square_error(crv, dvec);
	int idx = best_nudge(crv, dvec, amount);
	if (!idx) return false;

	execute_nudge(crv, idx, amount);
	double minerr = sum_square_error(crv, dvec);

	double lim = std::pow(10.0, (ii+2));
	double impr = sse - minerr;

	if ((impr * lim) < sse)
		return false;

	while (true)
	{
		Curve oth = crv;
		execute_nudge(oth, idx, amount);
		sse = sum_square_error(oth, dvec);

		if (sse >= minerr)
			break;

		if ((++count%64)==0)
		{
			std::cout << "N : " << ii << "  SSE : " << sse << "\r" << std::flush;
			if (AsynKB::HaveChar())
				return true;
		}

		minerr = sse;
		crv = oth;

	}
	updateBase(crv);
	return true;
}

#include "graph.h"

void fitting(const DataVec& dvec, std::string name)
{
	{
		std::ofstream ofs(name+"-data.txt");
		for (auto&& item : dvec)
		{
			ofs << item.size << "\t" << item.time << "\n";
		}
	}
	
	Curve crv;
	double amount = 0.01;
	int count=0, i=1;
	std::cout << std::endl;
	while (true)
	{
		bool ok = continuos_nudge(crv, dvec, amount, count, i);
		if (AsynKB::HaveChar()) break;
		if (!ok)
		{
			++i;
			if (i>=12) break;
			std::cout << "N : " << i << "  SSE : " << sum_square_error(crv, dvec) << "\r" << std::flush;
			amount *= 0.1;
		}
	}

	AsynKB::Clear();

	std::cout << "curve fitting for " << name << std::endl;
	std::cout << "base    " << crv.base    << std::endl; 
	std::cout << "linear  " << crv.linear  << std::endl;
	std::cout << "power   " << crv.power   << std::endl;
	std::cout << "pfactor " << crv.pfactor << std::endl;
	std::cout << std::endl << "SSE : " << sum_square_error(crv, dvec) << std::endl;

	Plot p;
	p.AddPoints(dvec.begin(), dvec.end());
	auto func = [&](double x) -> double
	{
		return executeCurve(crv, x);
	};
	p.SetFunction(func);
	Image img = p.generate(1024, 768);
	img.Save(name+".bmp");
}

*/





