/* hap/log-combinatorics.h -*- mode: c++ -*-
   Copyright (c) 2015-2026 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_LOG_COMBINATORICS_H_
#define HAP_LOG_COMBINATORICS_H_

/* Logarithms of counts that are too large to compute directly. Each
   function takes integer arguments of any type; a negative argument throws
   nyx::exception. A count of 0 has the logarithm -infinity. */

#include <nyx/concepts.h>
#include <nyx/exception.h>
#include <cstdint>
#include <iterator>
#include <limits>
#include <ranges>
#include <type_traits>

namespace hap {

/* Return log n!. */

double log_factorial(nyx::integer auto n);

/* Return log n!!. If n is even, then n!! = n * (n - 2) * ... * 2; if n is odd,
   then n!! = n * (n - 2) * ... * 3 * 1. */

double log_double_factorial(nyx::integer auto n);

/* Return log x_r, where x_r = (x - 1) ... (x - r + 1), the number of
   ordered selections of `r' elements from `x'. */

double log_falling_factorial(nyx::integer auto x, nyx::integer auto r);

/* Return `log (n, k)', where `(n, k)' is the binomial of `n' and `k'. */

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

namespace hap::internal {

/* Return `n' as a uint64_t, or throw if it's negative or too large. */
uint64_t to_uint64_t(nyx::integer auto n);

/* The functions above for 64-bit arguments. They're kept apart from the
   templates, which they would otherwise let other types, such as bool and
   double, convert to. */
extern double log_factorial(uint64_t n);
extern double log_double_factorial(uint64_t n);
extern double log_falling_factorial(uint64_t x, uint64_t r);
extern double log_binomial(uint64_t n, uint64_t k);

} /* namespace hap::internal */

template<> double hap::log_factorial<uint64_t>(uint64_t n);

uint64_t
hap::internal::to_uint64_t(nyx::integer auto n)
{
  if constexpr (std::is_signed_v<decltype(n)>) {
    if (n < 0)
      throw nyx::exception("argument out of range: {}",
	static_cast<int64_t>(n));
  }
  if constexpr (sizeof(n) > sizeof(uint64_t)) {
    if (n > std::numeric_limits<uint64_t>::max())
      throw nyx::exception("argument out of range");
  }
  return static_cast<uint64_t>(n);
}

double
hap::log_factorial(nyx::integer auto n)
{
  return internal::log_factorial(internal::to_uint64_t(n));
}

double
hap::log_double_factorial(nyx::integer auto n)
{
  return internal::log_double_factorial(internal::to_uint64_t(n));
}

double
hap::log_falling_factorial(nyx::integer auto x, nyx::integer auto r)
{
  return internal::log_falling_factorial(internal::to_uint64_t(x),
    internal::to_uint64_t(r));
}

double
hap::log_binomial(nyx::integer auto n, nyx::integer auto k)
{
  return internal::log_binomial(internal::to_uint64_t(n),
    internal::to_uint64_t(k));
}

double
hap::log_multinomial(nyx::integer auto... args)
{
  const uint64_t n = (uint64_t{0} + ... + internal::to_uint64_t(args));
  return internal::log_factorial(n)
    - (0.0 + ... + internal::log_factorial(internal::to_uint64_t(args)));
}

template<std::input_iterator I, std::sentinel_for<I> S>
  requires nyx::integer<std::iter_value_t<I>>
double
hap::log_multinomial(I first, S last)
{
  uint64_t n = 0;
  double p = 0;
  for ( ; first != last; ++first) {
    const auto k = internal::to_uint64_t(*first);
    n += k;
    p += internal::log_factorial(k);
  }
  return internal::log_factorial(n) - p;
}

template<std::ranges::input_range R>
  requires nyx::integer<std::ranges::range_value_t<R>>
double
hap::log_multinomial(R&& range)
{
  return log_multinomial(std::ranges::begin(range), std::ranges::end(range));
}

#endif /* HAP_LOG_COMBINATORICS_H_ */
