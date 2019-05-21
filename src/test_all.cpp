
#include "avl_vector.hpp"
#include "inline_vector.hpp"
#include "splice_list.hpp"
#include "test_item.hpp"

#include <iostream>
#include <list>
#include <vector>

constexpr std::size_t REP = 15;
constexpr std::size_t SZ = 1250;

using namespace std::literals;

constexpr std::size_t SML = (SZ * 3) / 2;
constexpr std::size_t BIG = SZ * 3;

namespace CT {
std::string nameof(std::vector<int>) { return "std::vector<int>"s; }
std::string nameof(avl::vector<int>) { return "avl::vector<int>"s; }

std::string nameof(std::vector<test_item>) { return "std::vector<test_item>"s; }
std::string nameof(avl::vector<test_item>) { return "avl::vector<test_item>"s; }
std::string nameof(std::list<test_item>) { return "std::list<test_item>"s; }
std::string nameof(splice_list<test_item>) { return "splice_list<test_item>"s; }

std::string nameof(inline_vector<test_item, SML>) {
  return "inline_vector<test_item," + std::to_string(SML) + ">"s;
}
std::string nameof(inline_vector<test_item, BIG>) {
  return "inline_vector<test_item," + std::to_string(BIG) + ">"s;
}

struct None {};
} // namespace CT

#include "container_tester.hpp"

void testsuit_performance() {
  using namespace std;
  using namespace CT;

  vector<int> vi;
  {
    bool ok = true;
    for (size_t i = 0; ok && (i < REP); ++i) {
      cout << "\r" << i << "   " << flush;
      vi.clear();
      vector<test_item> vti;
      list<test_item> lti;
      inline_vector<test_item, SML> ivtis;
      inline_vector<test_item, BIG> ivtib;
      splice_list<test_item> slti;
      avl::vector<test_item> avti;

#define ALL vi, vti, lti, ivtis, ivtib, slti, avti
      //#define ALL vi, vti, lti, avti

      fillup<>{}(SZ, ALL);
      if (ok)
        ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);

      insert<>{SZ}(ALL);
      if (ok)
        ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);
      erase<>{(SZ * 3) / 2}(ALL);
      if (ok)
        ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);

      if (ok)
        for (std::size_t j = 0; ok && (j < REP); ++j) {
          if (ok)
            insert<>{SZ}(ALL);
          if (ok)
            ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);
          if (ok)
            erase<>{SZ}(ALL);
          if (ok)
            ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);
          if (ok)
            nth_swap<>{SZ}(ALL);
          if (ok)
            ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);
        }

      if (ok)
        CT::sort<>{}(ALL);
      if (ok)
        ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);

      if (ok)
        CT::unique<>{}(ALL);
      if (ok)
        ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);

      for (size_t j = 0; ok && (j < REP); ++j) {
        if (ok)
          splice_merge<>{}(ALL);
        if (ok)
          ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);
        if (ok)
          ok = is_sorted(vi.begin(), vi.end());
      }

      if (ok)
        CT::remove<>{}(test_item{SZ / 2}, ALL);
      if (ok)
        binary_find_swap<>{}(test_item{SZ / 3}, test_item{2 * SZ / 3}, ALL);
      if (ok)
        CT::reverse<>{}(ALL);
      if (ok)
        ok = CT::integrity<>{}(ALL) && compare<>{}(ALL);

      if (!ok) {
        cout << "compare test failed" << endl;
        print<>{}(cout, ALL);
      }
#undef ALL

      if (ok) {
        ok = !test_item::error();
        if (!ok)
          cout << "move/copy test failed" << endl;
      }
    }
  }

  cout << "\r";
  auto rep = test_item::report();
  for (auto str : rep)
    cout << str << endl;
  if (rep.empty())
    cout << "move/delete: nothing to report" << endl;

  cout << endl;
  print<>{}(cout, vi);
  cout << endl;
  report_times<decltype(vi)>();
  // report_times(1000.0, "ms");
}
