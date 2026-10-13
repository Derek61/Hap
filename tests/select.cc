/* select.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/random.h>
#include <hap/select.h>
#include <nyx/command-line.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <map>
#include <print>
#include <set>
#include <vector>

void run_tests();

int
main(int argc, char** argv)
{
  nyx::command_line cl{nyx::argcount::none};
  if (auto exit = cl.parse_command_line(argc, argv); exit)
    return *exit;
  std::println(stderr, "Checking selection and shuffling.");
  run_tests();
  return EXIT_SUCCESS;
}

namespace {

using elements = std::vector<unsigned int>;

/* The number of samples in each statistical check. */
constexpr int n = 600000;

/* Return true if a count of `k' out of `trials' trials, each with
   probability `p', is within 5 standard deviations of its mean. For a
   correct generator, this fails with probability less than one in a
   million. */

bool
is_plausible(int k, int trials, double p)
{
  const double mean = trials * p;
  const double sd = std::sqrt(trials * p * (1 - p));
  return std::abs(k - mean) <= 5 * sd;
}

/* Return the elements from 1 to `count'. */

elements
one_to(unsigned int count)
{
  elements e(count);
  for (unsigned int k = 0; k < count; ++k)
    e[k] = k + 1;
  return e;
}

/* Return `e' sorted. */

elements
sorted(elements e)
{
  std::ranges::sort(e);
  return e;
}

/* A shuffle holds each element from 1 to `n' once, and every order of
   three elements is about equally likely. */

void
check_shuffle()
{
  assert(hap::shuffle(0).empty());
  assert(hap::shuffle(1) == one_to(1));
  for (unsigned int count : {2u, 10u, 52u, 1000u})
    assert(sorted(hap::shuffle(count)) == one_to(count));

  std::map<elements, int> orders;
  for (int k = 0; k < n; ++k)
    ++orders[hap::shuffle(3)];
  assert(orders.size() == 6);
  for (const auto& [order, count] : orders)
    assert(is_plausible(count, n, 1.0 / 6));
}

/* select_one() returns every value in [n, m], each about equally often,
   including at the ends of the range of unsigned int. */

void
check_select_one()
{
  assert(hap::select_one(5, 5) == 5);

  std::array<int, 6> counts = {};
  for (int k = 0; k < n; ++k) {
    const auto v = hap::select_one(3, 8);
    assert(v >= 3 && v <= 8);
    ++counts[v - 3];
  }
  for (int count : counts)
    assert(is_plausible(count, n, 1.0 / 6));

  constexpr auto max = std::numeric_limits<unsigned int>::max();
  bool low = false;
  bool high = false;
  for (int k = 0; k < 1000; ++k) {
    const auto v = hap::select_one(max - 1, max);
    assert(v == max - 1 || v == max);
    low = low || v == max - 1;
    high = high || v == max;
  }
  assert(low && high);
  bool zero = false;
  bool one = false;
  for (int k = 0; k < 1000; ++k) {
    const auto v = hap::select_one(0, 1);
    assert(v <= 1);
    zero = zero || v == 0;
    one = one || v == 1;
  }
  assert(zero && one);
  (void)hap::select_one(0, max);
}

/* A selection without replacement holds `k' different elements from 1 to
   `n'. Each element is equally likely to be chosen and equally likely to
   come first. Both methods are checked: the partial shuffle (when `k' is
   at least a quarter of `n') and Floyd's algorithm (when it's smaller). */

void
check_without_replacement()
{
  assert(hap::select_without_replacement(0, 0).empty());
  assert(hap::select_without_replacement(0, 10).empty());
  assert(sorted(hap::select_without_replacement(10, 10)) == one_to(10));

  struct sample
  {
    unsigned int k;
    unsigned int n;
  };
  for (auto [k, count] : {sample{3, 10}, sample{2, 20}, sample{1, 7}}) {
    std::vector<int> chosen(count);
    std::vector<int> first(count);
    for (int t = 0; t < n; ++t) {
      const auto s = hap::select_without_replacement(k, count);
      assert(s.size() == k);
      assert(std::set<unsigned int>(s.begin(), s.end()).size() == k);
      for (auto e : s) {
	assert(e >= 1 && e <= count);
	++chosen[e - 1];
      }
      ++first[s.front() - 1];
    }
    for (unsigned int e = 0; e < count; ++e) {
      assert(is_plausible(chosen[e], n, static_cast<double>(k) / count));
      assert(is_plausible(first[e], n, 1.0 / count));
    }
  }

  /* With a huge range, Floyd's algorithm needs space only for the elements
     chosen. */
  constexpr auto max = std::numeric_limits<unsigned int>::max();
  const auto s = hap::select_without_replacement(5, max);
  assert(s.size() == 5);
  assert(std::set<unsigned int>(s.begin(), s.end()).size() == 5);
  for (auto e : s)
    assert(e >= 1);
}

/* A selection with replacement holds `k' elements from 1 to `n', each
   chosen independently, so `k' may exceed `n'. */

void
check_with_replacement()
{
  assert(hap::select_with_replacement(0, 0).empty());
  assert(hap::select_with_replacement(4, 1) == elements(4, 1));

  std::array<int, 5> counts = {};
  int repeats = 0;
  for (int t = 0; t < n / 10; ++t) {
    const auto s = hap::select_with_replacement(10, 5);
    assert(s.size() == 10);
    for (auto e : s) {
      assert(e >= 1 && e <= 5);
      ++counts[e - 1];
    }
    repeats += std::set<unsigned int>(s.begin(), s.end()).size() < 10 ? 1 : 0;
  }
  for (int count : counts)
    assert(is_plausible(count, n, 1.0 / 5));
  assert(repeats == n / 10);
}

/* Equal seeds give equal results. */

void
check_seeding()
{
  hap::seed(31);
  const auto a = hap::shuffle(20);
  const auto b = hap::select_without_replacement(4, 1000);
  const auto c = hap::select_with_replacement(6, 9);
  const auto d = hap::select_one(1, 1000000);
  hap::seed(31);
  assert(hap::shuffle(20) == a);
  assert(hap::select_without_replacement(4, 1000) == b);
  assert(hap::select_with_replacement(6, 9) == c);
  assert(hap::select_one(1, 1000000) == d);
}

} /* namespace */

void
run_tests()
{
  check_shuffle();
  check_select_one();
  check_without_replacement();
  check_with_replacement();
  check_seeding();
}
