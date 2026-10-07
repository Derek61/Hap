/* die.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/die.h>
#include <hap/random.h>
#include <nyx/command-line.h>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <print>
#include <vector>

void run_tests();

int
main(int argc, char** argv)
{
  nyx::command_line cl{nyx::argcount::none};
  if (auto exit = cl.parse_command_line(argc, argv); exit)
    return *exit;
  std::println(stderr, "Checking a single die.");
  run_tests();
  return EXIT_SUCCESS;
}

namespace {

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

/* A default die has 6 sides and hasn't been rolled. */

void
check_defaults()
{
  die d;
  assert(d.sides() == 6);
  assert(d.value() == 0);

  die d20{20};
  assert(d20.sides() == 20);
  assert(d20.value() == 0);
}

/* Every roll is in [1, sides], and value() reports the last roll. */

void
check_range()
{
  for (unsigned int sides : {1u, 2u, 6u, 7u, 20u, 100u}) {
    die d{sides};
    for (int k = 0; k < 10000; ++k) {
      const auto v = d.roll();
      assert(v >= 1 && v <= sides);
      assert(d.value() == v);
    }
  }
}

/* Each face comes up about equally often, including for numbers of sides
   that aren't powers of 2. */

void
check_uniformity()
{
  for (unsigned int sides : {2u, 6u, 7u, 20u}) {
    die d{sides};
    std::vector<int> counts(sides);
    for (int k = 0; k < n; ++k)
      ++counts[d.roll() - 1];
    for (int count : counts)
      assert(is_plausible(count, n, 1.0 / sides));
  }
}

/* Copying a die copies its number of sides and its value. */

void
check_copy()
{
  die a{12};
  a.roll();
  die b = a;
  assert(b.sides() == 12);
  assert(b.value() == a.value());
}

/* Equal seeds give equal rolls. */

void
check_seeding()
{
  die d{100};
  hap::seed(5);
  std::vector<unsigned int> first;
  for (int k = 0; k < 1000; ++k)
    first.push_back(d.roll());
  hap::seed(5);
  for (int k = 0; k < 1000; ++k)
    assert(d.roll() == first[static_cast<size_t>(k)]);
}

} /* namespace */

void
run_tests()
{
  check_defaults();
  check_range();
  check_uniformity();
  check_copy();
  check_seeding();
}
