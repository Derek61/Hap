/* hap/estimate.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/estimate.h>
#include <hap/random.h>
#include <nyx/utility.h>
#include <algorithm>
#include <cmath>
#include <exception>
#include <thread>
#include <vector>

using estimator = hap::estimator;
using result = hap::estimator::result;

namespace {

/* Run trials [first, last) of `scenario' and return the number of
   successes. */

uint64_t
run_trials(estimator::scenario scenario, const hap::internal::seed_state& s,
  uint64_t first, uint64_t last)
{
  uint64_t successes = 0;
  for (auto k = first; k != last; ++k) {
    hap::internal::start_trial(s, k);
    if (scenario())
      ++successes;
  }
  return successes;
}

} /* namespace */

estimator::estimator(unsigned int threads)
  : thread_count_{threads}
{
  if (thread_count_ == max_threads)
    thread_count_ = std::thread::hardware_concurrency();
  thread_count_ = std::max(thread_count_, 1u);
}

unsigned int
estimator::thread_count() const
{
  return thread_count_;
}

auto
estimator::estimate(uint64_t trials, scenario scenario) const -> result
{
  NYX_PRECONDITION(trials > 0);

  const auto n = std::clamp<uint64_t>(thread_count(), 1, trials);
  const auto s = hap::internal::current_seed();

  if (n == 1) {
    hap::internal::preserve_state saved;
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

result::result(uint64_t trials, uint64_t successes)
  : trials{trials}, successes{successes}
{
  const auto n = static_cast<double>(trials);
  probability = static_cast<double>(successes) / n;
  standard_error = std::sqrt(probability * (1 - probability) / n);
}
