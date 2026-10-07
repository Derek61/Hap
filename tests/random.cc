/* random.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/die.h>
#include <hap/random.h>
#include <nyx/command-line.h>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <print>
#include <thread>
#include <vector>

void run_tests();

int
main(int argc, char** argv)
{
  nyx::command_line cl{nyx::argcount::none};
  if (auto exit = cl.parse_command_line(argc, argv); exit)
    return *exit;
  std::println(stderr, "Checking the global seed.");
  run_tests();
  return EXIT_SUCCESS;
}

namespace {

/* Return `n' rolls of a 1000-sided die on the calling thread. */

std::vector<unsigned int>
rolls(int n)
{
  hap::die d{1000};
  std::vector<unsigned int> values;
  for (int k = 0; k < n; ++k)
    values.push_back(d.roll());
  return values;
}

/* Before any seed is set, one is chosen once and then reported
   consistently. This must run before anything sets the seed. */

void
check_unset_seed()
{
  const auto s = hap::seed();
  assert(hap::seed() == s);
  (void)rolls(10);
  assert(hap::seed() == s);
}

/* Setting the seed is reported back, and restarts the calling thread's
   sequence, so equal seeds give equal sequences and different seeds give
   different ones. */

void
check_set_seed()
{
  hap::seed(42);
  assert(hap::seed() == 42);
  const auto first = rolls(1000);

  hap::seed(42);
  assert(rolls(1000) == first);

  hap::seed(43);
  assert(hap::seed() == 43);
  assert(rolls(1000) != first);
}

/* Setting the seed partway through a sequence restarts it from the
   beginning. */

void
check_restart()
{
  hap::seed(7);
  const auto whole = rolls(20);
  hap::seed(7);
  (void)rolls(5);
  hap::seed(7);
  assert(rolls(20) == whole);
}

/* Separate threads draw different sequences under the same seed. */

void
check_threads()
{
  hap::seed(11);
  std::vector<unsigned int> a;
  std::vector<unsigned int> b;
  {
    std::jthread first{[&] { a = rolls(100); }};
  }
  {
    std::jthread second{[&] { b = rolls(100); }};
  }
  assert(a.size() == 100 && b.size() == 100);
  assert(a != b);
  assert(a != rolls(100));
}

} /* namespace */

void
run_tests()
{
  check_unset_seed();
  check_set_seed();
  check_restart();
  check_threads();
}
