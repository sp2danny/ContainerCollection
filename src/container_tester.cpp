
#include "container_tester.hpp"

#include <ctime>

namespace CT {
bool inited = false;
std::default_random_engine generator;
// std::chrono::high_resolution_clock::time_point t1;
std::clock_t t1;
std::map<std::string, std::map<std::string, double>> time_data;
} // namespace CT

void CT::clear_times() { time_data.clear(); }

void CT::init() {
  if (inited)
    return;
  inited = true;
  auto tm = std::chrono::system_clock::now();
  generator.seed((unsigned int)tm.time_since_epoch().count());
}

void CT::start_clock() {
  t1 = clock(); // std::chrono::high_resolution_clock::now();
}

double CT::stop_clock() {
  auto diff = clock() - t1;
  return diff * 1000.0 / CLOCKS_PER_SEC;
}
