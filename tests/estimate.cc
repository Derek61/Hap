/* estimate.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/coin.h>
#include <hap/dice.h>
#include <hap/die.h>
#include <hap/estimate.h>
#include <hap/random.h>
#include <nyx/command-line.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <print>
#include <stdexcept>
#include <type_traits>
#include <vector>

void run_tests();

int
main(int argc, char** argv)
{
  nyx::command_line cl{nyx::argcount::none};
  if (auto exit = cl.parse_command_line(argc, argv); exit)
    return *exit;
  std::println(stderr, "Checking probability estimates.");
  run_tests();
  return EXIT_SUCCESS;
}

namespace {

using hap::estimator;

static_assert(!std::is_convertible_v<unsigned int, estimator>);

/* The number of trials in each statistical check. */
constexpr uint64_t n = 1000000;

/* Roll two dice; toss a coin, and on heads reroll the lower die, on tails
   the higher one. Succeed if the sum went up. The exact probability is
   5/12. */

bool
reroll()
{
  hap::dice d{2};
  const auto before = d.roll();
  hap::coin c;
  auto& die = c.toss() == hap::coin::Heads ? d.lowest() : d.highest();
  die.roll();
  return d.sum() > before;
}

/* Roll two dice and succeed on a 7. The exact probability is 1/6. */

bool
seven()
{
  hap::dice d{2};
  return d.roll() == 7;
}

/* Return true if the estimate in `r' is within 5 standard errors of
   `p'. */

bool
is_plausible(const estimator::result& r, double p)
{
  const double se = std::sqrt(p * (1 - p) / static_cast<double>(r.trials));
  return std::abs(r.probability - p) <= 5 * se;
}

/* The thread count is at least 1, and `max_threads' means as many as the
   hardware supports. */

void
check_thread_count()
{
  assert(estimator{}.thread_count() == 1);
  assert(estimator{0}.thread_count() == 1);
  assert(estimator{1}.thread_count() == 1);
  assert(estimator{7}.thread_count() == 7);
  assert(estimator{estimator::max_threads}.thread_count() >= 1);
  assert(estimator{estimator::max_threads}.thread_count()
	 != estimator::max_threads);
}

/* A default result is all zero. */

void
check_default_result()
{
  const estimator::result r;
  assert(r.trials == 0);
  assert(r.successes == 0);
  assert(r.probability == 0);
  assert(r.standard_error == 0);
}

/* Scenarios that always or never succeed give exact results. */

void
check_certain()
{
  estimator e{4};
  const auto yes = e.estimate(1001, [] { return true; });
  assert(yes.trials == 1001);
  assert(yes.successes == 1001);
  assert(yes.probability == 1);
  assert(yes.standard_error == 0);

  const auto no = e.estimate(1001, [] { return false; });
  assert(no.successes == 0);
  assert(no.probability == 0);
  assert(no.standard_error == 0);
}

/* The probability and standard error follow from the counts. */

void
check_result_arithmetic()
{
  const auto r = estimator{}.estimate(10007, seven);
  const double p = static_cast<double>(r.successes) / 10007;
  assert(r.trials == 10007);
  assert(r.probability == p);
  assert(r.standard_error == std::sqrt(p * (1 - p) / 10007));
}

/* Estimates agree with known probabilities. */

void
check_accuracy()
{
  estimator e{estimator::max_threads};
  assert(is_plausible(e.estimate(n, seven), 1.0 / 6));
  assert(is_plausible(e.estimate(n, reroll), 5.0 / 12));
}

/* For a given seed, the result doesn't depend on the number of threads,
   including counts that don't divide the number of trials evenly and
   counts larger than the number of trials. */

void
check_thread_independence()
{
  hap::seed(1234);
  const auto expected = estimator{}.estimate(100003, reroll).successes;
  for (unsigned int threads : {2u, 3u, 7u, 16u, estimator::max_threads})
    assert(estimator{threads}.estimate(100003, reroll).successes
	   == expected);

  const auto few = estimator{8}.estimate(3, reroll);
  assert(few.trials == 3);
  assert(few.successes == estimator{}.estimate(3, reroll).successes);
}

/* Equal seeds give equal results; different seeds, different results. */

void
check_seeding()
{
  estimator e{4};
  hap::seed(1);
  const auto a = e.estimate(100000, reroll).successes;
  hap::seed(1);
  assert(e.estimate(100000, reroll).successes == a);
  hap::seed(2);
  assert(e.estimate(100000, reroll).successes != a);
}

/* Each trial draws its own numbers, and trial k draws the same numbers
   however many trials are run. The scenario records each trial's first
   roll; this runs on one thread, so the recording is safe. */

void
check_trial_streams()
{
  std::vector<unsigned int> long_run;
  std::vector<unsigned int> short_run;
  auto recorder = [](std::vector<unsigned int>& values)
  {
    return [&values]
    {
      hap::die d{1000000};
      values.push_back(d.roll());
      return true;
    };
  };

  hap::seed(77);
  (void)estimator{}.estimate(1000, recorder(long_run));
  (void)estimator{}.estimate(10, recorder(short_run));
  assert(long_run.size() == 1000);
  assert(std::equal(short_run.begin(), short_run.end(), long_run.begin()));

  /* With a million faces, consecutive trials drawing the same first roll
     by chance should almost never happen: about 0.001 times in this
     run. */
  int repeats = 0;
  for (size_t k = 1; k < long_run.size(); ++k)
    repeats += long_run[k] == long_run[k - 1] ? 1 : 0;
  assert(repeats < 5);
}

/* An estimate on the calling thread doesn't disturb the sequence that
   thread draws from outside it. */

void
check_preserved_sequence()
{
  hap::die d{1000};
  hap::seed(9);
  std::vector<unsigned int> expected;
  for (int k = 0; k < 10; ++k)
    expected.push_back(d.roll());

  hap::seed(9);
  std::vector<unsigned int> actual;
  for (int k = 0; k < 5; ++k)
    actual.push_back(d.roll());
  (void)estimator{}.estimate(1000, reroll);
  for (int k = 0; k < 5; ++k)
    actual.push_back(d.roll());
  assert(actual == expected);
}

/* An exception thrown by a scenario reaches the caller, from one thread or
   from several. */

void
check_exceptions()
{
  for (unsigned int threads : {1u, 4u}) {
    bool caught = false;
    try {
      (void)estimator{threads}.estimate(100, [] [[noreturn]] () -> bool {
	throw std::runtime_error{"scenario failed"};
      });
    } catch (const std::runtime_error&) {
      caught = true;
    }
    assert(caught);
  }

  /* Only some trials fail. */
  bool caught = false;
  try {
    (void)estimator{4}.estimate(1000, [] {
      hap::die d{100};
      if (d.roll() == 100)
	throw std::runtime_error{"rolled 100"};
      return true;
    });
  } catch (const std::runtime_error&) {
    caught = true;
  }
  assert(caught);
}

} /* namespace */

void
run_tests()
{
  check_thread_count();
  check_default_result();
  check_certain();
  check_result_arithmetic();
  check_accuracy();
  check_thread_independence();
  check_seeding();
  check_trial_streams();
  check_preserved_sequence();
  check_exceptions();
}
