
#include "test_item.hpp"

namespace detail {
unsigned char un[8] = {22, 33, 44, 55, 66, 77, 88, 99};
unsigned char pr[8] = {12, 13, 14, 15, 16, 17, 18, 19};
unsigned char mf[8] = {89, 78, 67, 56, 45, 34, 23, 12};
unsigned char dl[8] = {98, 87, 76, 65, 54, 43, 32, 21};

std::vector<std::string> report;

unsigned long long cc = 0, dc = 0;
} // namespace detail

void add_report(std::string str) { detail::report.push_back(std::move(str)); }

using namespace detail;
using namespace std::literals;

const std::vector<std::string> &test_item::report() {
  if (cc != dc) {
    if (cc > dc)
      add_report("under-deleted : "s + std::to_string(cc - dc));
    if (cc < dc)
      add_report("over-deleted : "s + std::to_string(dc - cc));
  }
  return detail::report;
}

bool test_item::error() { return !detail::report.empty(); }

#include <cstring>

test_item::test_item() {
  if (std::memcmp(magic, un, 8) == 0)
    add_report("creating object in other (uninitialized) object");
  if (std::memcmp(magic, pr, 8) == 0)
    add_report("creating object in other (proper) object");
  if (std::memcmp(magic, mf, 8) == 0)
    add_report("creating object in other (moved-from) object");

  std::memcpy(magic, un, 8);
  state = uninitialized;
  ++cc;
}

test_item::test_item(int i) {
  if (std::memcmp(magic, un, 8) == 0)
    add_report("creating object in other (uninitialized) object");
  if (std::memcmp(magic, pr, 8) == 0)
    add_report("creating object in other (proper) object");
  if (std::memcmp(magic, mf, 8) == 0)
    add_report("creating object in other (moved-from) object");

  value = i;
  std::memcpy(magic, pr, 8);
  state = proper;
  ++cc;
}

test_item::test_item(const test_item &other) {
  if (std::memcmp(magic, un, 8) == 0)
    add_report("creating object in other (uninitialized) object");
  if (std::memcmp(magic, pr, 8) == 0)
    add_report("creating object in other (proper) object");
  if (std::memcmp(magic, mf, 8) == 0)
    add_report("creating object in other (moved-from) object");

  if (other.state == uninitialized)
    add_report("copying uninitialized object");
  if (other.state == movedfrom)
    add_report("copying movedfrom object");
  if (other.state == deleted)
    add_report("copying deleted object");
  if (std::memcmp(other.magic, pr, 8) != 0)
    add_report("copying garbled object");

  value = other.value;
  std::memcpy(magic, pr, 8);
  state = proper;
  ++cc;
}

test_item::test_item(test_item &&other) {
  if (std::memcmp(magic, un, 8) == 0)
    add_report("creating object in other (uninitialized) object");
  if (std::memcmp(magic, pr, 8) == 0)
    add_report("creating object in other (proper) object");
  if (std::memcmp(magic, mf, 8) == 0)
    add_report("creating object in other (moved-from) object");

  if (other.state == uninitialized)
    add_report("copying uninitialized object");
  if (other.state == movedfrom)
    add_report("copying movedfrom object");
  if (other.state == deleted)
    add_report("copying deleted object");
  if (std::memcmp(other.magic, pr, 8) != 0)
    add_report("copying garbled object");

  value = other.value;
  std::memcpy(magic, pr, 8);
  state = proper;

  other.state = movedfrom;
  std::memcpy(other.magic, mf, 8);
  ++cc;
}

test_item &test_item::operator=(const test_item &other) {
  if (this == &other)
    return *this;

  if (std::memcmp(magic, dl, 8) == 0)
    add_report("assigning object in deleted space");
  if (state == deleted)
    add_report("assigning object in deleted space");

  if (other.state == uninitialized)
    add_report("copying uninitialized object");
  if (other.state == movedfrom)
    add_report("copying movedfrom object");
  if (other.state == deleted)
    add_report("copying deleted object");
  if (std::memcmp(other.magic, pr, 8) != 0)
    add_report("copying garbled object");

  value = other.value;
  std::memcpy(magic, pr, 8);
  state = proper;

  return *this;
}

test_item &test_item::operator=(test_item &&other) noexcept {
  if (this == &other)
    return *this;

  if (std::memcmp(magic, dl, 8) == 0)
    add_report("assigning object in deleted space");
  if (state == deleted)
    add_report("assigning object in deleted space");

  if (other.state == uninitialized)
    add_report("copying uninitialized object");
  if (other.state == movedfrom)
    add_report("copying movedfrom object");
  if (other.state == deleted)
    add_report("copying deleted object");
  if (std::memcmp(other.magic, pr, 8) != 0)
    add_report("copying garbled object");

  value = other.value;
  std::memcpy(magic, pr, 8);
  state = proper;

  other.state = movedfrom;
  std::memcpy(other.magic, mf, 8);

  return *this;
}

test_item &test_item::operator=(int val) {
  if (std::memcmp(magic, dl, 8) == 0)
    add_report("assigning object in deleted space");
  if (state == deleted)
    add_report("assigning object in deleted space");

  value = val;
  std::memcpy(magic, pr, 8);
  state = proper;

  return *this;
}

test_item::~test_item() {
  if (std::memcmp(magic, dl, 8) == 0)
    add_report("double delete");
  if (state == deleted)
    add_report("double delete");
  std::memcpy(magic, dl, 8);
  state = deleted;
  ++dc;
}

std::ostream &operator<<(std::ostream &out, const test_item &item) {
  if (item.state == test_item::uninitialized)
    add_report("reading uninitialized object");
  if (item.state == test_item::movedfrom)
    add_report("reading movedfrom object");
  if (item.state == test_item::deleted)
    add_report("reading deleted object");
  if (std::memcmp(item.magic, pr, 8) != 0)
    add_report("reading garbled object");
  out << item.value;
  return out;
}

test_item::operator int() const {
  if (state == test_item::uninitialized)
    add_report("reading uninitialized object");
  if (state == test_item::movedfrom)
    add_report("reading movedfrom object");
  if (state == test_item::deleted)
    add_report("reading deleted object");
  if (std::memcmp(magic, pr, 8) != 0)
    add_report("reading garbled object");
  return value;
}

long long test_item::compare(const test_item &rhs) const {
  auto &lhs = *this;

  if (lhs.state == test_item::uninitialized)
    add_report("reading uninitialized object");
  if (lhs.state == test_item::movedfrom)
    add_report("reading movedfrom object");
  if (lhs.state == test_item::deleted)
    add_report("reading deleted object");
  if (std::memcmp(lhs.magic, pr, 8) != 0)
    add_report("reading garbled object");

  if (rhs.state == test_item::uninitialized)
    add_report("reading uninitialized object");
  if (rhs.state == test_item::movedfrom)
    add_report("reading movedfrom object");
  if (rhs.state == test_item::deleted)
    add_report("reading deleted object");
  if (std::memcmp(rhs.magic, pr, 8) != 0)
    add_report("reading garbled object");

  return (long long)(lhs.value) - (long long)(rhs.value);
}
