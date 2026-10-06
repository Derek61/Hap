/* hap/estimate.h -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_ESTIMATE_H_
#define HAP_ESTIMATE_H_

/* Estimate the probability of an event by simulation. A scenario is a
   function that plays out one trial and returns true if the event
   occurred:

     hap::estimator e;
     auto r = e.estimate(1'000'000, []
     {
       hap::dice d{2};
       return d.roll() == 7;
     });

   To spread the trials over several threads, construct the estimator
   with a thread count, such as `hap::estimator{4}' or
   `hap::estimator{hap::estimator::max_threads}'.

   Trial k draws from a sequence determined only by the global seed and by
   k, so for a given seed the result is the same however many threads are
   used. Since trials may run on several threads at once, a scenario must
   create the objects it uses and must not change anything shared between
   trials. */

#include <hap/random.h>
#include <cstdint>

namespace hap {

class estimator
{
public:
  class result;

  /* Pass to `estimator(unsigned int)' to use the max number of threads
     allowed on the system. */
  static constexpr unsigned int max_threads = static_cast<unsigned int>(-1);

  /* An estimator that runs on a single thread. */
  estimator() = default;

  /* An estimator that runs on `threads' threads, or on as many as the
     hardware supports if `threads' is `max_threads'. A count of 0 is
     treated as 1. */
  explicit estimator(unsigned int threads);

  /* Return the number of threads this estimator uses. */
  unsigned int thread_count() const;

  /* Run `count' trials of `scenario' and estimate the probability that it
     returns true. `count' must be nonzero. If there are fewer trials than
     threads, use only as many threads as there are trials. */
  template<typename Scenario> result estimate(uint64_t count,
    const Scenario& scenario);

private:
  unsigned int thread_count_ = 1;

  template<typename Scenario> static uint64_t run_trials(const Scenario&,
    const internal::seed_state&, uint64_t, uint64_t);
};

/* The outcome of a simulation. */

class estimator::result
{
public:
  uint64_t trials = 0;		/* The number of trials run. */
  uint64_t successes = 0;	/* The number of successful trials. */
  double probability = 0;	/* Estimated probability of success. */
  double standard_error = 0;	/* Standard error of the estimate. */

  result() = default;

private:
  friend class estimator;
  result(uint64_t, uint64_t);
};

} /* namespace hap */

/* Implementation. */

#include <nyx/utility.h>
#include <algorithm>
#include <exception>
#include <thread>
#include <type_traits>
#include <vector>

template<typename Scenario>
auto
hap::estimator::estimate(uint64_t trials, const Scenario& scenario) -> result
{
  static_assert(std::is_invocable_r_v<bool, const Scenario&>,
    "a scenario takes no arguments, returns bool, and must not "
    "modify its captures");

  NYX_PRECONDITION(trials > 0);

  const auto n = std::clamp<uint64_t>(thread_count(), 1, trials);
  const auto s = internal::current_seed();

  if (n == 1) {
    internal::preserve_state saved;
    return result(trials, run_trials(scenario, s, 0, trials));
  }

  /* Divide the trials into `n' blocks whose sizes differ by at most 1. The
     workers join when `workers' is destroyed or earlier if creating a
     thread throws. */

  std::vector<uint64_t> successes(n);
  std::vector<std::exception_ptr> exceptions(n);
  {
    std::vector<std::jthread> workers;
    const auto size = trials / n;
    const auto extra = trials % n;
    for (uint64_t k = 0, first = 0; k < n; ++k) {
      const auto last = first + size + (k < extra ? 1 : 0);
      workers.emplace_back([&, k, first, last] {
	try {
	  successes[k] = run_trials(scenario, s, first, last);
	} catch (...) {
	  exceptions[k] = std::current_exception();
	}
      });
      first = last;
    }
  }
  for (const auto& exception : exceptions) {
    if (exception != nullptr)
      std::rethrow_exception(exception);
  }
  uint64_t total = 0;
  for (auto k : successes)
    total += k;
  return result(trials, total);
}

/* Run trials [first, last) of `scenario' and return the number of
   successes. */

template<typename Scenario>
uint64_t
hap::estimator::run_trials(const Scenario& scenario,
  const internal::seed_state& s, uint64_t first, uint64_t last)
{
  uint64_t successes = 0;
  for (auto k = first; k != last; ++k) {
    start_trial(s, k);
    if (scenario())
      ++successes;
  }
  return successes;
}

#endif /* HAP_ESTIMATE_H_ */
