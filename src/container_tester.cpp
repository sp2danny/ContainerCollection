
#include "container_tester.hpp"

namespace CT
{
	bool inited = false;
	std::default_random_engine generator;
	std::chrono::high_resolution_clock::time_point t1;
	std::map<std::string, std::map<std::string, double>> time_data;
}

void CT::clear_times()
{
	time_data.clear();
}

void CT::init()
{
	if (inited) return;
	inited = true;
	auto tm = std::chrono::system_clock::now();
	generator.seed((unsigned int)tm.time_since_epoch().count());
}

void CT::start_clock()
{
	t1 = std::chrono::high_resolution_clock::now();
}

double CT::stop_clock()
{
	std::chrono::duration<double, std::ratio<1,1>> diff = std::chrono::high_resolution_clock::now() - t1;
	return diff.count();
}

