/* log-combinatorics.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/log-combinatorics.h>
#include <nyx/command-line.h>
#include <nyx/exception.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <list>
#include <print>
#include <vector>

void run_tests();

int
main(int argc, char** argv)
{
  nyx::command_line cl{nyx::argcount::none};
  if (auto exit = cl.parse_command_line(argc, argv); exit)
    return *exit;
  std::println(stderr, "Checking logarithms of counts.");
  run_tests();
  return EXIT_SUCCESS;
}

namespace {

constexpr double inf = std::numeric_limits<double>::infinity();
constexpr uint64_t max = std::numeric_limits<uint64_t>::max();

/* Return true if `a' is within a relative error of `tolerance' of `b'. */

bool
is_close(double a, double b, double tolerance = 1e-12)
{
  if (b == 0)
    return std::abs(a) < tolerance;
  return std::abs(a - b) <= tolerance * std::abs(b);
}

/* The relative error allowed for a falling factorial or binomial with
   argument `x'. Summing logs is used only while the error from
   subtracting log-factorials would be larger, so the error can reach
   about epsilon times the square root of `x'. */

double
tolerance(uint64_t x)
{
  return std::max(1e-12, 1e-15 * std::sqrt(static_cast<double>(x)));
}

/* Return the log of `n' computed in long double. */

double
log_of(uint64_t n)
{
  return static_cast<double>(std::log(static_cast<long double>(n)));
}

/* Return log x (x - 1) ... (x - r + 1), summed in long double. */

double
direct_log_falling_factorial(uint64_t x, uint64_t r)
{
  long double sum = 0;
  for (uint64_t i = 0; i < r; ++i)
    sum += std::log(static_cast<long double>(x - i));
  return static_cast<double>(sum);
}

/* log_multinomial() accepts a range of `T'. */
template<typename T> concept accepts_range =
  requires(T t) { hap::log_multinomial(t); };

/* log_factorial() accepts a `T'. */
template<typename T> concept accepts_value =
  requires(T t) { hap::log_factorial(t); };

/* Return true if calling `f' throws nyx::exception. */

template<typename F>
bool
throws(F f)
{
  try {
    (void)f();
  } catch (const nyx::exception&) {
    return true;
  }
  return false;
}

/* log n! matches n! for every n whose factorial fits in 64 bits. */

void
check_factorial()
{
  uint64_t f = 1;
  for (uint64_t n = 0; n <= 20; ++n) {
    f *= std::max<uint64_t>(n, 1);
    assert(is_close(hap::log_factorial(n), log_of(f)));
  }
  assert(is_close(hap::log_factorial(1000000),
    direct_log_falling_factorial(1000000, 1000000)));
}

/* log n!! matches n!! for every n whose double factorial fits in 64 bits,
   for both parities. */

void
check_double_factorial()
{
  std::array<uint64_t, 34> f = {};
  f[0] = 1;
  f[1] = 1;
  for (uint64_t n = 2; n < f.size(); ++n)
    f[n] = n * f[n - 2];
  for (uint64_t n = 0; n < f.size(); ++n)
    assert(is_close(hap::log_double_factorial(n), log_of(f[n])));

  /* (2k)!! (2k - 1)!! = (2k)!. */
  for (uint64_t k : {1u, 10u, 1000u, 1000000u})
    assert(is_close(hap::log_double_factorial(2 * k)
      + hap::log_double_factorial(2 * k - 1), hap::log_factorial(2 * k)));
}

/* The falling factorial is accurate on both sides of the switch between
   summing logs and subtracting log-factorials, is 0 when r exceeds x, and
   handles the largest x. */

void
check_falling_factorial()
{
  assert(hap::log_falling_factorial(0, 0) == 0);
  assert(hap::log_falling_factorial(10, 0) == 0);
  assert(is_close(hap::log_falling_factorial(10, 3), log_of(720)));
  assert(is_close(hap::log_falling_factorial(52, 52),
    hap::log_factorial(52)));
  assert(hap::log_falling_factorial(3, 4) == -inf);
  assert(hap::log_falling_factorial(0, 1) == -inf);

  for (uint64_t x : {10u, 1000u, 1000000u, 1000000000u})
    for (uint64_t r : {1u, 2u, 31u, 32u, 33u, 999u, 1000u, 31622u, 31623u})
      if (r <= x && r <= 1000000)
	assert(is_close(hap::log_falling_factorial(x, r),
	  direct_log_falling_factorial(x, r), tolerance(x)));

  assert(is_close(hap::log_falling_factorial(max, 3),
    direct_log_falling_factorial(max, 3)));
}

/* log C(n, k) matches Pascal's triangle wherever it fits in 64 bits, is
   symmetric, is accurate for large n, and is 0 when k exceeds n. */

void
check_binomial()
{
  std::vector<uint64_t> row = {1};
  for (uint64_t n = 0; n <= 60; ++n) {
    for (uint64_t k = 0; k <= n; ++k)
      assert(is_close(hap::log_binomial(n, k), log_of(row[k])));
    std::vector<uint64_t> next(row.size() + 1, 1);
    for (size_t k = 1; k < row.size(); ++k)
      next[k] = row[k - 1] + row[k];
    row = std::move(next);
  }

  for (uint64_t n : {1000u, 1000000u, 1000000000u})
    for (uint64_t k : {1u, 2u, 13u, 500u, 100000u})
      if (k <= n) {
	const double expected = direct_log_falling_factorial(n, k)
	  - direct_log_falling_factorial(k, k);
	assert(is_close(hap::log_binomial(n, k), expected, tolerance(n)));
	assert(hap::log_binomial(n, k) == hap::log_binomial(n, n - k));
      }

  assert(hap::log_binomial(3, 5) == -inf);
  assert(hap::log_binomial(0, 0) == 0);
}

/* The three forms of log_multinomial agree, and a multinomial with two
   groups is a binomial. */

void
check_multinomial()
{
  const std::vector<int> hands = {13, 13, 13, 13};
  const double deals = hap::log_multinomial(13, 13, 13, 13);
  assert(hap::log_multinomial(hands) == deals);
  assert(hap::log_multinomial(hands.begin(), hands.end()) == deals);
  assert(is_close(deals, hap::log_binomial(52, 13) + hap::log_binomial(39, 13)
    + hap::log_binomial(26, 13)));

  assert(is_close(hap::log_multinomial(30, 70), hap::log_binomial(100, 30)));
  assert(hap::log_multinomial() == 0);
  assert(hap::log_multinomial(std::vector<int>{}) == 0);

  const std::list<unsigned char> groups = {1, 2};
  assert(is_close(hap::log_multinomial(groups), log_of(3)));
}

/* Every integer type is accepted; a negative argument, or one too large
   for 64 bits, throws; and a range of non-integers doesn't compile. */

void
check_arguments()
{
  const signed char a = 5;
  const unsigned short b = 60000;
  const unsigned short c = 50000;
  const auto d = static_cast<__int128>(20);
  assert(is_close(hap::log_factorial(a), log_of(120)));
  assert(is_close(hap::log_falling_factorial(b, c),
    direct_log_falling_factorial(b, c)));
  assert(is_close(hap::log_factorial(d), hap::log_factorial(20)));

  assert(throws([] { return hap::log_factorial(-1); }));
  assert(throws([] { return hap::log_double_factorial(-1); }));
  assert(throws([] { return hap::log_falling_factorial(-1, 0); }));
  assert(throws([] { return hap::log_falling_factorial(1, -1); }));
  assert(throws([] { return hap::log_binomial(-3, 1); }));
  assert(throws([] { return hap::log_multinomial(1, -1); }));
  assert(throws([] {
    return hap::log_factorial(static_cast<unsigned __int128>(1) << 64);
  }));

  static_assert(accepts_range<std::vector<int>>);
  static_assert(!accepts_range<std::vector<double>>);
  static_assert(accepts_value<unsigned char>);
  static_assert(!accepts_value<bool>);
  static_assert(!accepts_value<char>);
  static_assert(!accepts_value<double>);
}

} /* namespace */

void
run_tests()
{
  check_factorial();
  check_double_factorial();
  check_falling_factorial();
  check_binomial();
  check_multinomial();
  check_arguments();
}
