/* coin.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/coin.h>
#include <hap/random.h>
#include <nyx/command-line.h>
#include <array>
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
  std::println(stderr, "Checking coin tosses.");
  run_tests();
  return EXIT_SUCCESS;
}

namespace {

using hap::coin;

/* The number of tosses in each statistical check. */
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

/* A coin can be created showing either face, and every query agrees with
   the last toss. */

void
check_faces()
{
  const coin h{coin::Heads};
  assert(h.face() == coin::Heads);
  assert(h.heads() && !h.tails());
  const coin t{coin::Tails};
  assert(t.face() == coin::Tails);
  assert(t.tails() && !t.heads());

  coin c;
  assert(c.heads() != c.tails());
  for (int k = 0; k < 1000; ++k) {
    const auto f = c.toss();
    assert(f == c.face());
    assert(c.heads() == (f == coin::Heads));
    assert(c.tails() == (f == coin::Tails));
    assert(c.heads() != c.tails());
  }
}

/* A new coin has been tossed: about half of new coins show heads. */

void
check_initial_toss()
{
  int heads = 0;
  for (int k = 0; k < n; ++k)
    heads += coin{}.heads() ? 1 : 0;
  assert(is_plausible(heads, n, 0.5));
}

/* About half the tosses are heads. */

void
check_balance()
{
  coin c;
  int heads = 0;
  for (int k = 0; k < n; ++k)
    heads += c.toss() == coin::Heads ? 1 : 0;
  assert(is_plausible(heads, n, 0.5));
}

/* Successive tosses are independent: each of the four outcomes of a pair
   occurs about a quarter of the time. A coin uses one bit of each 64-bit
   draw at a time, so this also checks the step from one draw to the
   next. */

void
check_pairs()
{
  coin c;
  std::array<int, 4> counts = {};
  for (int k = 0; k < n; ++k) {
    const int first = c.toss() == coin::Heads ? 1 : 0;
    const int second = c.toss() == coin::Heads ? 1 : 0;
    ++counts[static_cast<size_t>(2 * first + second)];
  }
  for (int count : counts)
    assert(is_plausible(count, n, 0.25));
}

/* Separate coins on one thread share its sequence: tossing two coins in
   turn gives the same faces as tossing one coin twice as often. The coins
   are created before the seed is set, so their initial tosses don't
   count. */

void
check_shared_sequence()
{
  coin c;
  hap::seed(3);
  std::vector<coin::Face> one;
  for (int k = 0; k < 200; ++k)
    one.push_back(c.toss());

  coin a;
  coin b;
  hap::seed(3);
  for (size_t k = 0; k < 200; k += 2) {
    assert(a.toss() == one[k]);
    assert(b.toss() == one[k + 1]);
  }
}

/* Equal seeds give equal tosses. */

void
check_seeding()
{
  coin c;
  hap::seed(17);
  std::vector<coin::Face> first;
  for (int k = 0; k < 1000; ++k)
    first.push_back(c.toss());
  hap::seed(17);
  for (int k = 0; k < 1000; ++k)
    assert(c.toss() == first[static_cast<size_t>(k)]);
}

} /* namespace */

void
run_tests()
{
  check_faces();
  check_initial_toss();
  check_balance();
  check_pairs();
  check_shared_sequence();
  check_seeding();
}
