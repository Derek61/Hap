/* hap/estimate.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/estimate.h>
#include <algorithm>
#include <cmath>
#include <thread>

using estimator = hap::estimator;
using result = hap::estimator::result;

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

result::result(uint64_t trials, uint64_t successes)
  : trials{trials}, successes{successes}
{
  const auto n = static_cast<double>(trials);
  probability = static_cast<double>(successes) / n;
  standard_error = std::sqrt(probability * (1 - probability) / n);
}
