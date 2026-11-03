/* hap/log-combinatorics.h -*- mode: c++ -*-
   Copyright (c) 2015-2026 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_LOG_COMBINATORICS_H_
#define HAP_LOG_COMBINATORICS_H_

/* Logarithms of combinatorial functions. */

#include <nyx/concepts.h>
#include <nyx/exception.h>
#include <cstdint>
#include <iterator>
#include <limits>
#include <ranges>
#include <type_traits>

namespace hap {

/* Each function throws `nyx::exception' if an argument is negative or
   doesn’t fit in 64 bits. */

/* Return log n!. */

double log_factorial(nyx::integer auto n);

/* Return log n!!. We define 0!! = 1!! = 1, and n!! = n * (n - 2)!! for
   n >= 2. */

double log_double_factorial(nyx::integer auto n);

/* Return log (x)_r, where (x)_r = x (x - 1) ... (x - r + 1), the number of
   ordered selections of `r' elements from `x'. If r > x, return -∞. */

double log_falling_factorial(nyx::integer auto x, nyx::integer auto r);

/* Return log C(n, k), where C(n, k) is the binomial coefficient, the number
   of ways to choose `k' elements from `n'. If k > n, return -∞. */

double log_binomial(nyx::integer auto n, nyx::integer auto k);

/* Return the logarithm of the multinomial coefficient
     (k_1 + ... + k_m)! / (k_1! ... k_m!),
   the number of ways to divide k_1 + ... + k_m elements into groups of
   k_1, ..., k_m elements. With no arguments, return 0. */

double log_multinomial(nyx::integer auto... args);

template<std::input_iterator I, std::sentinel_for<I> S>
  requires nyx::integer<std::iter_value_t<I>>
double log_multinomial(I first, S last);

template<std::ranges::input_range R>
  requires nyx::integer<std::ranges::range_value_t<R>>
double log_multinomial(R&& range);

} /* namespace hap */

/* Implementation. */

template<> double hap::log_factorial(uint64_t n);
template<> double hap::log_double_factorial(uint64_t n);
template<> double hap::log_falling_factorial(uint64_t x, uint64_t r);
template<> double hap::log_binomial(uint64_t n, uint64_t k);

namespace hap::internal {

/* Return `n' as a uint64_t, or throw if it's negative or too large. */

uint64_t
to_uint64(nyx::integer auto n)
{
  if constexpr (std::is_signed_v<decltype(n)>) {
    if (n < 0)
      throw nyx::exception("argument out of range: {}", n);
  }
  if constexpr (sizeof(n) > sizeof(uint64_t)) {
    if (n > std::numeric_limits<uint64_t>::max())
      throw nyx::exception("argument out of range: {}", n);
  }
  return static_cast<uint64_t>(n);
}

} /* namespace hap::internal */

double
hap::log_factorial(nyx::integer auto n)
{
  return log_factorial(internal::to_uint64(n));
}

double
hap::log_double_factorial(nyx::integer auto n)
{
  return log_double_factorial(internal::to_uint64(n));
}

double
hap::log_falling_factorial(nyx::integer auto x, nyx::integer auto r)
{
  return log_falling_factorial(internal::to_uint64(x), internal::to_uint64(r));
}

double
hap::log_binomial(nyx::integer auto n, nyx::integer auto k)
{
  return log_binomial(internal::to_uint64(n), internal::to_uint64(k));
}

double
hap::log_multinomial(nyx::integer auto... args)
{
  const uint64_t n = (uint64_t{0} + ... + internal::to_uint64(args));
  return log_factorial(n) - (0.0 + ... + log_factorial(args));
}

template<std::input_iterator I, std::sentinel_for<I> S>
  requires nyx::integer<std::iter_value_t<I>>
double
hap::log_multinomial(I first, S last)
{
  double p = 0;
  uint64_t n = 0;
  for ( ; first != last; ++first) {
    const auto k = internal::to_uint64(*first);
    n += k;
    p += log_factorial(k);
  }
  return log_factorial(n) - p;
}

template<std::ranges::input_range R>
  requires nyx::integer<std::ranges::range_value_t<R>>
double
hap::log_multinomial(R&& range)
{
  return log_multinomial(std::ranges::begin(range), std::ranges::end(range));
}

#endif /* HAP_LOG_COMBINATORICS_H_ */
