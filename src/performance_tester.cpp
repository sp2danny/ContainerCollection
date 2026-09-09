
#include "avl_vector.hpp"
#include "container_tester.hpp"
#include "inline_vector.hpp"
#include "splice_list.hpp"

// #include "asyn_kb.h"
#include "graph.h"

#include <iostream>
#include <list>
#include <string>
#include <vector>

struct Data
{
	std::size_t size;
	double      insert_time;
	double      splice_time;
	double      sort_time;
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

	auto mkdata = [sz](auto cont) -> Data {
		Data data;
		data.size        = sz;
		auto name        = CT::nameof(cont);
		data.insert_time = CT::time_data[name]["insert_nth"];
		data.insert_time += CT::time_data[name]["erase_nth"];
		data.splice_time = CT::time_data[name]["splice_merge"];
		data.sort_time   = CT::time_data[name]["sort"];
		return data;
	};

	vectorData.push_back(mkdata(vi));
	treeData.push_back(mkdata(ti));
	listData.push_back(mkdata(li));

	if (last)
		CT::report_times<>();

	if (last)
		std::cout << "done testing " << std::endl;
}

extern void fitting(const DataVec&, std::string);

void testsuit()
{
	avl::vector<int> avi;
	for (int i = 1; i <= 22; ++i)
		avi.push_back(i);
	avi.print_tree(std::cout, false, true);

	// AsynKB::Start();
	// all_test(50000, false);
	for (int j = 0; j < 60; ++j)
	{
		int i = j / 3;
#ifndef NDEBUG
		if (j % 3)
			continue;
#endif
		std::cout << i << "." << (j % 3) << "\r" << std::flush;
		all_test(100 + i * 10 + j % 3);
		all_test(350 + i * 35 + j % 3);
		all_test(1000 + i * 100 + j % 3);
		all_test(3500 + i * 350 + j % 3);
		all_test(10000 + i * 1000 + j % 3);

#ifdef NDEBUG
		all_test(35000 + i * 3500 + j % 3);
		all_test(3610 + i * 350 + j % 3);
		all_test(3720 + i * 350 + j % 3);
		all_test(10330 + i * 1000 + j % 3);
		all_test(10660 + i * 1000 + j % 3);
		all_test(36100 + i * 3500 + j % 3);
		all_test(37200 + i * 3500 + j % 3);
#endif
	}
	std::cout << 20 << "\r" << std::flush;
	all_test(10000, true);

	auto mkimg = [](const DataVec& dv, std::string name) -> void {
		MultiPlot mp;
		for (auto&& itm : dv)
		{
			mp.AddPoint({255, 127, 127}, (double)itm.size, itm.insert_time);
			mp.AddPoint({127, 255, 127}, (double)itm.size, itm.splice_time);
			mp.AddPoint({127, 127, 255}, (double)itm.size, itm.sort_time);
		}
		Image img = mp.generate(1024, 768);
		img.Save(name);
	};

	mkimg(vectorData, "VectorData.bmp");
	mkimg(treeData, "TreeData.bmp");
	mkimg(listData, "ListData.bmp");

	auto mkimg2 = [](const DataVec& vec, const DataVec& tree, const DataVec& lst) -> void {
		MultiPlot mp;
		for (auto&& itm : vec)
			mp.AddPoint({255, 127, 127}, (double)itm.size, itm.insert_time + itm.splice_time + itm.sort_time);
		for (auto&& itm : tree)
			mp.AddPoint({127, 255, 127}, (double)itm.size, itm.insert_time + itm.splice_time + itm.sort_time);
		for (auto&& itm : lst)
			mp.AddPoint({127, 127, 255}, (double)itm.size, itm.insert_time + itm.splice_time + itm.sort_time);
		Image img = mp.generate(1024, 768);
		img.Save("all.bmp");
	};

	mkimg2(vectorData, treeData, listData);

}

