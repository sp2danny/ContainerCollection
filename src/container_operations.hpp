
#pragma once

#ifndef NDEBUG
#include <iostream>
#endif

#include <algorithm>
#include <iterator>
#include <type_traits>

namespace CO {

template <typename C1, typename C2, typename C3>
void merge_unique(C1 &c1, C2 &c2, C3 &c3) {
  if (c1.empty() && c2.empty())
    return;

  auto i1 = c1.begin();
  auto i2 = c2.begin();
  decltype(c3.begin()) last;

  auto take_1 = [&]() {
    last = c3.insert(c3.end(), *i1);
    ++i1;
  };
  auto take_2 = [&]() {
    last = c3.insert(c3.end(), *i2);
    ++i2;
  };

  if (c1.empty())
    take_2();
  else if (c2.empty())
    take_1();
  else if ((*i1) < (*i2))
    take_1();
  else
    take_2();
  while (true) {
    bool ae1 = (i1 == c1.end());
    bool ae2 = (i2 == c2.end());
    if (ae1 && ae2)
      break;
    if (ae1) {
      while (i2 != c2.end()) {
        if ((*last) == (*i2))
          ++i2;
        else
          take_2();
      }
      break;
    } else if (ae2) {
      while (i1 != c1.end()) {
        if ((*last) == (*i1))
          ++i1;
        else
          take_1();
      }
      break;
    } else {
      if ((*i1) < (*i2)) {
        if ((*last) == (*i1))
          ++i1;
        else
          take_1();
      } else {
        if ((*last) == (*i2))
          ++i2;
        else
          take_2();
      }
    }
  }
}

namespace detail {
struct pick_4 {};
struct pick_3 : pick_4 {};
struct pick_2 : pick_3 {};
struct pick_1 : pick_2 {};

template <typename C1>
auto sort(pick_1, C1 &c1) -> decltype(c1.sort(), void()) {
#ifdef FULL_DIAG
  std::cerr << "attempting: member sort <>\r";
#endif
  c1.sort();
}

template <typename C1> void sort(pick_2, C1 &c1) {
#ifdef FULL_DIAG
  std::cerr << "attempting: std::sort <>\r";
#endif
  std::sort(c1.begin(), c1.end());
}

template <typename C1>
auto unique(pick_1, C1 &c1) -> decltype(c1.unique(), void()) {
#ifdef FULL_DIAG
  std::cerr << "attempting: member unique <>\r";
#endif
  c1.unique();
}

template <typename C1> void unique(pick_2, C1 &c1) {
#ifdef FULL_DIAG
  std::cerr << "attempting: std::unique <>\r";
#endif
  typename C1::iterator b, e, i;
  b = c1.begin();
  e = c1.end();
  i = std::unique(b, e);
#ifdef FULL_DIAG
  std::cerr << "attempting: member erase range <>\r";
#endif
  c1.erase(i, e);
}

template <typename C1, typename Itm>
auto remove(pick_1, C1 &c1, const Itm &itm)
    -> decltype(c1.remove(itm), void()) {
#ifdef FULL_DIAG
  std::cerr << "attempting: member remove on " << typeid(C1).name()
            << std::endl;
#endif
  c1.remove(itm);
}

template <typename C1, typename Itm>
auto remove(pick_2, C1 &c1, const Itm &itm) -> std::enable_if_t<
    std::is_base_of<std::random_access_iterator_tag,
                    typename std::iterator_traits<
                        decltype(std::begin(c1))>::iterator_category>::value> {
#ifdef FULL_DIAG
  std::cerr << "attempting: std::remove, then member erase on "
            << typeid(C1).name() << std::endl;
#endif
  auto itr = std::remove(c1.begin(), c1.end(), itm);
  c1.erase(itr, c1.end());
}

template <typename C1, typename Itm>
auto remove(pick_3, C1 &c1, const Itm &itm) {
#ifdef FULL_DIAG
  std::cerr << "attempting: manual remove on " << typeid(C1).name()
            << std::endl;
#endif
  for (auto iter = c1.begin(); iter != c1.end();) {
    if (itm == *iter)
      iter = c1.erase(iter);
    else
      ++iter;
  }
}

template <typename C1, typename Itm>
auto binary_find(pick_1, C1 &&c1, const Itm &itm)
    -> std::pair<bool, decltype(c1.binary_find(itm))> {
  auto itr = c1.binary_find(itm);
  if (itr != c1.end())
    return {true, itr};
  else
    return {false, {}};
}

template <typename C1, typename Itm>
auto binary_find(pick_2, C1 &&c1, const Itm &itm)
    -> std::pair<bool, decltype(c1.lower_bound(itm))> {
  auto iter = c1.lower_bound(itm);
  if (iter == c1.end())
    return {false, {}};
  if (itm == *iter)
    return {true, iter};
  else
    return {false, {}};
}

template <typename C1, typename Itm>
auto binary_find(pick_3, C1 &&c1, const Itm &itm)
    -> std::pair<bool, decltype(c1.begin())> {
  using std::lower_bound;
  auto iter = lower_bound(c1.begin(), c1.end(), itm);
  if (iter == c1.end())
    return {false, iter};
  if (itm == *iter)
    return {true, iter};
  else
    return {false, iter};
}

template <typename C1, typename Itm>
auto binary_find(pick_4, const C1 &c1, const Itm &itm)
    -> std::pair<bool, typename C1::const_iterator> {
  using std::lower_bound;
  auto iter = lower_bound(c1.begin(), c1.end(), itm);
  if (iter == c1.end())
    return {false, iter};
  if (itm == *iter)
    return {true, iter};
  else
    return {false, iter};
}

template <typename Cont, typename It>
auto splice(pick_1, Cont &src, It sb, It se, Cont &dst, It d)
    -> decltype(dst.splice(d, src, sb, se), void()) {
  dst.splice(d, src, sb, se);
}

template <typename Cont, typename It>
auto splice(pick_2, Cont &src, It sb, It se, Cont &dst, It d) -> void {
  It i = sb;
  while (i != se) {
    d = dst.insert(d, std::move(*i));
    ++d;
    ++i;
  }
  src.erase(sb, se);
}

template <typename Cont>
auto merge(pick_1, Cont &c1, Cont &c2) -> decltype(c1.merge(c2), void()) {
  c1.merge(c2);
}

template <typename Cont> auto merge(pick_2, Cont &c1, Cont &c2) -> void {
  Cont dst;
  std::merge(c1.begin(), c1.end(), c2.begin(), c2.end(),
             std::back_inserter(dst));
  c1.swap(dst);
}

template <typename Cont>
auto nth(pick_1, Cont &c1, std::size_t idx) -> decltype(c1.nth(idx)) {
  return c1.nth(idx);
}
template <typename Cont>
auto nth(pick_2, Cont &c1, std::size_t idx) -> decltype(c1.begin() + idx) {
  return c1.begin() + idx;
}
template <typename Cont>
auto nth(pick_3, Cont &c1, std::size_t idx) -> decltype(c1.begin()) {
  return std::next(c1.begin(), idx);
}

template <typename Cont>
auto integrity(pick_1, Cont &c1) -> decltype(c1.integrity()) {
  return c1.integrity();
}
template <typename Cont> auto integrity(pick_2, Cont &c1) -> bool {
  std::size_t n = 0, sz = c1.size();
  auto it = c1.begin();
  while (it != c1.end()) {
    auto tmp = it;
    ++tmp;
    --tmp;
    if (tmp != it)
      return false;
    ++n;
    ++it;
    if (n > sz)
      return false;
  }
  return n == sz;
}

template <typename Cont>
auto reverse(pick_1, Cont &c1) -> decltype(c1.reverse(), void()) {
  c1.reverse();
}

template <typename Cont>
auto reverse(pick_2, Cont &c1)
    -> decltype(std::reverse(c1.begin(), c1.end()), void()) {
  std::reverse(c1.begin(), c1.end());
}

} // namespace detail

template <typename Cont> void reverse(Cont &c1) {
  return detail::reverse(detail::pick_1{}, c1);
}

template <typename Cont> auto integrity(Cont &c1) {
  return detail::integrity(detail::pick_1{}, c1);
}

template <typename Cont> auto nth(Cont &c1, std::size_t idx) {
  return detail::nth(detail::pick_1{}, c1, idx);
}

template <typename C1> void sort(C1 &c1) { detail::sort(detail::pick_1{}, c1); }

template <typename C1> void unique(C1 &c1) {
  detail::unique(detail::pick_1{}, c1);
}

template <typename C1, typename Itm> void remove(C1 &c1, const Itm &itm) {
  detail::remove(detail::pick_1{}, c1, itm);
}

template <typename C1, typename Itm> auto binary_find(C1 &&c1, const Itm &itm) {
  return detail::binary_find(detail::pick_1{}, c1, itm);
}

template <typename Cont, typename It>
void splice(Cont &src, It sb, It se, Cont &dst, It d) {
  detail::splice(detail::pick_1{}, src, sb, se, dst, d);
}

template <typename Cont> void merge(Cont &c1, Cont &c2) {
  detail::merge(detail::pick_1{}, c1, c2);
}

} // namespace CO

using namespace CO;
