/* reroll.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

/* Roll two dice and note the sum. Toss a coin: on heads, reroll the lower
   die; on tails, reroll the higher one. Estimate the probability that the
   new sum exceeds the old one. The exact answer is 5/12.

   The estimate is made on one thread and then on several. For a given
   seed, the two agree exactly. */

#include <hap/coin.h>
#include <hap/dice.h>
#include <hap/estimate.h>
#include <hap/random.h>
#include <nyx/command-line.h>
#include <cstdint>
#include <cstdlib>
#include <print>

int
main(int argc, char** argv)
{
  using enum hap::coin::Face;
  using nyx::argument;

  uint64_t trials = 10'000'000;
  unsigned int threads = hap::estimator::max_threads;

  nyx::option_set options = {
    {"seed", 's', argument::required, "N", "Use N as the global seed."},
    {"trials", 'n', argument::required, "N",
     "Run N trials (default 10000000)."},
    {"threads", 't', argument::required, "N",
     "Compare with a run on N threads (default: all)."}
  };
  nyx::command_line cl{options, nyx::argcount::none};
  if (auto exit = cl.parse_command_line(argc, argv); exit)
    return *exit;
  for (const auto& option : cl.get_selected_options()) {
    switch (option.get_identifier()) {
    case 's':
      if (auto s = option.as<uint64_t>(); s) {
	hap::seed(*s);
      } else {
	cl.invalid_option_value(option);
	return EXIT_FAILURE;
      }
      break;
    case 'n':
      if (auto n = option.as<uint64_t>(); n && *n > 0) {
	trials = *n;
      } else {
	cl.invalid_option_value(option);
	return EXIT_FAILURE;
      }
      break;
    case 't':
      if (auto t = option.as<unsigned int>(); t && *t > 0) {
	threads = *t;
      } else {
	cl.invalid_option_value(option);
	return EXIT_FAILURE;
      }
      break;
    default:
      return EXIT_FAILURE;
    }
  }

  auto scenario = []
  {
    hap::dice d{2};
    const auto before = d.roll();
    hap::coin c;
    auto& die = c.toss() == Heads ? d.lowest() : d.highest();
    die.roll();
    return d.sum() > before;
  };

  std::println("Seed: {}", hap::seed());
  for (auto t : {1u, threads}) {
    auto e = hap::estimator(t);
    const auto r = e.estimate(trials, scenario);
    std::println("{:2} threads: {:.6f} ± {:.6f}", e.thread_count(),
      r.probability, r.standard_error);
  }
  std::println("Exact:      {:.6f}", 5.0 / 12);
  return EXIT_SUCCESS;
}
