/* hap/log-combinatorics.cc -*- mode: c++ -*-
   Copyright (c) 2017-2026 Derek B. Clegg
   All rights reserved. */

/* This must be defined for <math.h> to declare `lgamma_r', the threadsafe
   version of `std::lgamma'. `lgamma_r' returns the sign of Γ(𝑥) through
   its second argument instead of setting the global variable `signgam'. */

#define _REENTRANT

#include <hap/log-combinatorics.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>

template<>
double
hap::log_factorial(uint64_t n)
{
  int sign;
  return lgamma_r(static_cast<double>(n) + 1, &sign);
}

/* n!! is 2^k k! when n = 2k, and (2k + 1)! / (2^k k!) when n = 2k + 1. */

template<>
double
hap::log_double_factorial(uint64_t n)
{
  const auto k = n / 2;
  const double d = static_cast<double>(k) * std::numbers::ln2
    + log_factorial(k);
  return n % 2 == 0 ? d : log_factorial(n) - d;
}

/* The product is x! / (x - r)!, which is 0 when `r' exceeds `x'. When `r'
   is small compared with `x', both log-factorials are much larger than
   their difference; to avoid loss of precision, sum the logs directly. This
   only happens when `r' is less than about the square root of `x'. */

template<>
double
hap::log_falling_factorial(uint64_t x, uint64_t r)
{
  if (r > x) {
    return -std::numeric_limits<double>::infinity();
  } else if (r == 0) {
    return 0;
  } else if (r >= x / r) {
    return log_factorial(x) - log_factorial(x - r);
  } else {
    double sum = 0;
    for (uint64_t k = 0; k < r; ++k)
      sum += std::log(static_cast<double>(x - k));
    return sum;
  }
}

/* C(n, k) = C(n, n - k) = (n)_k / k!. */

template<>
double
hap::log_binomial(uint64_t n, uint64_t k)
{
  if (k > n)
    return -std::numeric_limits<double>::infinity();
  k = std::min(k, n - k);
  return log_falling_factorial(n, k) - log_factorial(k);
}
