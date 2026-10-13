/* hap/log-combinatorics.cc -*- mode: c++ -*-
   Copyright (c) 2017-2026 Derek B. Clegg
   All rights reserved. */

#include <hap/log-combinatorics.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>

template<>
double
hap::log_factorial<uint64_t>(uint64_t n)
{
  return std::lgamma(static_cast<double>(n) + 1);
}

double
hap::internal::log_factorial(uint64_t n)
{
  return std::lgamma(static_cast<double>(n) + 1);
}

/* n!! is 2^k k! when n = 2k, and (2k + 1)! / (2^k k!) when n = 2k + 1. */

double
hap::internal::log_double_factorial(uint64_t n)
{
  const uint64_t k = n / 2;
  const double d = static_cast<double>(k) * std::numbers::ln2
    + log_factorial(k);
  return n % 2 == 0 ? d : log_factorial(n) - d;
}

/* The product is x! / (x - r)!, which is 0 when `r' exceeds `x'. When `r'
   is small compared with `x', both log-factorials are much larger than
   their difference, which loses precision, so sum the `r' logs directly;
   this happens only when `r' is less than about the square root of `x'. */

double
hap::internal::log_falling_factorial(uint64_t x, uint64_t r)
{
  if (r > x)
    return -std::numeric_limits<double>::infinity();
  if (r > 0 && r >= x / r)
    return log_factorial(x) - log_factorial(x - r);
  double sum = 0;
  for (uint64_t i = 0; i < r; ++i)
    sum += std::log(static_cast<double>(x - i));
  return sum;
}

/* C(n, k) = C(n, n - k) = (n)_k / k!. */

double
hap::internal::log_binomial(uint64_t n, uint64_t k)
{
  if (k > n)
    return -std::numeric_limits<double>::infinity();
  k = std::min(k, n - k);
  return log_falling_factorial(n, k) - log_factorial(k);
}
