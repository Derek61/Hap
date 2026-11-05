/* dice.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/dice.h>
#include <hap/random.h>
#include <nyx/command-line.h>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <print>

void run_tests();

int
main(int argc, char** argv)
{
  nyx::command_line cl{nyx::argcount::none};
  if (auto exit = cl.parse_command_line(argc, argv); exit)
    return *exit;
  std::println(stderr, "Checking sets of dice.");
  run_tests();
  return EXIT_SUCCESS;
}

namespace {

using hap::dice;
using hap::die;

/* The number of rolls in each statistical check. */
constexpr int n = 1000000;

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

/* Call `f' and check that it throws std::bad_alloc. */

template<typename Function>
void
check_bad_alloc(Function f)
{
  try {
    f();
  } catch (const std::bad_alloc&) {
    return;
  }
  assert(false);
}

/* Return the sum of the dice's values, computed independently of
   dice::sum(). */

unsigned int
total(const dice& d)
{
  unsigned int s = 0;
  for (size_t k = 0; k < d.count(); ++k)
    s += d[k].value();
  return s;
}

/* A set holds the requested number of dice, each with the requested number
   of sides, already rolled. */

void
check_construction()
{
  dice d{3};
  assert(d.count() == 3);
  for (const auto& x : d) {
    assert(x.sides() == 6);
    assert(x.value() >= 1 && x.value() <= 6);
  }
  assert(d.sum() == total(d));

  dice e{5, 20};
  assert(e.count() == 5);
  for (const auto& x : e) {
    assert(x.sides() == 20);
    assert(x.value() >= 1 && x.value() <= 20);
  }
  assert(e.sum() == total(e));

  dice none{0};
  assert(none.count() == 0);
  assert(none.sum() == 0);
  assert(none.roll() == 0);
}

/* At most `capacity' dice fit in a set. */

void
check_capacity()
{
  dice full{dice::capacity};
  assert(full.count() == dice::capacity);
  check_bad_alloc([] { dice d{dice::capacity + 1}; });
  check_bad_alloc([] { dice d{dice::capacity + 1, 6}; });
}

/* roll() returns the sum, and sum() stays correct after one die is
   rerolled. */

void
check_sum()
{
  dice d{4, 10};
  for (int k = 0; k < 1000; ++k) {
    const auto s = d.roll();
    assert(s == total(d));
    assert(d.sum() == s);
    d[2].roll();
    assert(d.sum() == total(d));
  }
}

/* lowest() and highest() return the first die with the lowest and the
   highest value, through both the non-const and const overloads. */

void
check_extremes()
{
  dice d{5};
  for (int k = 0; k < 10000; ++k) {
    d.roll();
    size_t lo = 0;
    size_t hi = 0;
    for (size_t j = 1; j < d.count(); ++j) {
      if (d[j].value() < d[lo].value())
	lo = j;
      if (d[j].value() > d[hi].value())
	hi = j;
    }
    assert(&d.lowest() == &d[lo]);
    assert(&d.highest() == &d[hi]);

    const dice& c = d;
    assert(&c.lowest() == &c[lo]);
    assert(&c.highest() == &c[hi]);
  }
}

/* Iteration visits the same dice as indexing, in order. */

void
check_iteration()
{
  dice d{6};
  d.roll();
  size_t k = 0;
  for (auto& x : d)
    assert(&x == &d[k++]);
  assert(k == d.count());

  const dice& c = d;
  k = 0;
  for (const auto& x : c)
    assert(&x == &c[k++]);
  assert(k == c.count());
}

/* The sum of two dice has the familiar triangular distribution:
   P(s) = (6 - |s - 7|) / 36 for s in [2, 12]. */

void
check_distribution()
{
  dice d{2};
  std::array<int, 13> counts = {};
  for (int k = 0; k < n; ++k)
    ++counts[d.roll()];
  assert(counts[0] == 0 && counts[1] == 0);
  for (int s = 2; s <= 12; ++s) {
    const double p = (6 - std::abs(s - 7)) / 36.0;
    assert(is_plausible(counts[static_cast<size_t>(s)], n, p));
  }
}

/* The dice in a new set are rolled independently: the sum of a new set of
   two dice has the triangular distribution. Rolling one die and copying it
   would give only even sums. */

void
check_initial_roll()
{
  for (auto make : {+[] { return dice{2}; }, +[] { return dice{2, 6}; }}) {
    std::array<int, 13> counts = {};
    for (int k = 0; k < n; ++k)
      ++counts[make().sum()];
    assert(counts[0] == 0 && counts[1] == 0);
    for (int s = 2; s <= 12; ++s) {
      const double p = (6 - std::abs(s - 7)) / 36.0;
      assert(is_plausible(counts[static_cast<size_t>(s)], n, p));
    }
  }
}

/* Equal seeds give equal rolls. */

void
check_seeding()
{
  dice d{3, 8};
  hap::seed(99);
  std::array<unsigned int, 100> first = {};
  for (auto& s : first)
    s = d.roll();
  hap::seed(99);
  for (auto s : first)
    assert(d.roll() == s);
}

} /* namespace */

void
run_tests()
{
  check_construction();
  check_capacity();
  check_sum();
  check_extremes();
  check_iteration();
  check_distribution();
  check_initial_roll();
  check_seeding();
}
