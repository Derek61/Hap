/* reroll.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

/* Roll two dice and note the sum. Toss a coin: on heads, reroll the lower
   die; on tails, reroll the higher one. Estimate the probability that the
   new sum exceeds the old one. The exact answer is 5/12. */

#include <hap/coin.h>
#include <hap/dice.h>
#include <hap/estimate.h>
#include <cstdint>
#include <cstdlib>
#include <print>

int
main()
{
  using enum hap::coin::Face;

  auto scenario = []
  {
    hap::dice d{2};
    const auto before = d.roll();
    hap::coin c;
    auto& die = c.toss() == Heads ? d.lowest() : d.highest();
    die.roll();
    return d.sum() > before;
  };

  constexpr uint64_t trials = 10'000'000;
  std::println("Seed: {:#x}", hap::seed());
  for (auto t : {1u, hap::estimator::max_threads}) {
    auto e = hap::estimator(t);
    const auto r = e.estimate(trials, scenario);
    std::println("{:2} threads: {:.6f} ± {:.6f}", e.thread_count(),
      r.probability, r.standard_error);
  }
  std::println("Exact:      {:.6f}", 5.0 / 12);
  return EXIT_SUCCESS;
}
