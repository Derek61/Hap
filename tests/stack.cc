/* stack.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/random.h>
#include <hap/stack.h>
#include <nyx/command-line.h>
#include <nyx/exception.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include <map>
#include <print>
#include <ranges>
#include <utility>
#include <vector>

void run_tests();

int
main(int argc, char** argv)
{
  nyx::command_line cl{nyx::argcount::none};
  if (auto exit = cl.parse_command_line(argc, argv); exit)
    return *exit;
  std::println(stderr, "Checking stacks of cards.");
  run_tests();
  return EXIT_SUCCESS;
}

namespace {

using stack = hap::stack<int>;

/* Return true if `s' holds `cards', top first. */

bool
equals(const stack& s, std::initializer_list<int> cards)
{
  return std::ranges::equal(s, cards);
}

/* Call `f' and check that it throws nyx::exception. */

template<typename Function>
void
check_throws(Function f)
{
  try {
    f();
  } catch (const nyx::exception&) {
    return;
  }
  assert(false);
}

/* Return true if a count of `k' out of `trials' trials, each with
   probability `p', is within 5 standard deviations of its mean. For a
   correct generator, this fails with probability less than one in a
   million. */

bool
is_plausible(int k, int trials, double p)
{
  const double mean = trials * p;
  const double sd = std::sqrt(trials * p * (1 - p));
  return std::abs(k - mean) <= 5 * sd;
}

/* A stack is built top first, from a list or from a range. */

void
check_construction()
{
  stack empty;
  assert(empty.empty());
  assert(empty.count() == 0);

  stack s{1, 2, 3};
  assert(!s.empty());
  assert(s.count() == 3);
  assert(s.top() == 1);
  assert(s[0] == 1 && s[1] == 2 && s[2] == 3);

  const std::vector<int> v{4, 5, 6};
  stack t{v.begin(), v.end()};
  assert(equals(t, {4, 5, 6}));
}

/* Dealing takes cards from the top. */

void
check_dealing()
{
  stack s{1, 2, 3, 4, 5};
  assert(s.deal_one_card() == 1);
  assert(equals(s, {2, 3, 4, 5}));

  auto hand = s.deal(3);
  assert(equals(hand, {2, 3, 4}));
  assert(equals(s, {5}));

  auto none = s.deal(0);
  assert(none.empty());
  assert(equals(s, {5}));

  auto last = s.deal(1);
  assert(equals(last, {5}));
  assert(s.empty());
}

/* Discarding removes cards from the top. */

void
check_discarding()
{
  stack s{1, 2, 3, 4};
  s.discard(2);
  assert(equals(s, {3, 4}));
  s.discard(0);
  assert(equals(s, {3, 4}));
  s.discard_all();
  assert(s.empty());
}

/* Asking for more cards than a stack holds throws, and leaves the stack
   unchanged. */

void
check_too_few()
{
  stack s{1, 2};
  check_throws([&] { (void)s.deal(3); });
  check_throws([&] { s.discard(3); });
  check_throws([&] { (void)s[2]; });
  assert(equals(s, {1, 2}));

  stack empty;
  check_throws([&] { (void)empty.top(); });
  check_throws([&] { (void)empty.deal_one_card(); });
}

/* Single cards and whole stacks can be added to either end. A stack added
   by copy is unchanged; one added by move is left empty. */

void
check_adding()
{
  stack s{3, 4};
  s.add_to_top(2);
  s.add_to_bottom(5);
  assert(equals(s, {2, 3, 4, 5}));

  const stack top{0, 1};
  s.add_to_top(top);
  assert(equals(s, {0, 1, 2, 3, 4, 5}));
  assert(equals(top, {0, 1}));

  const stack bottom{6, 7};
  s.add_to_bottom(bottom);
  assert(equals(s, {0, 1, 2, 3, 4, 5, 6, 7}));
  assert(equals(bottom, {6, 7}));

  stack t{3, 4};
  stack moved_top{1, 2};
  t.add_to_top(std::move(moved_top));
  assert(equals(t, {1, 2, 3, 4}));
  assert(moved_top.empty());

  stack moved_bottom{5, 6};
  t.add_to_bottom(std::move(moved_bottom));
  assert(equals(t, {1, 2, 3, 4, 5, 6}));
  assert(moved_bottom.empty());
}

/* begin() and end() go from top to bottom; rbegin() and rend() from bottom
   to top. */

void
check_iteration()
{
  const stack s{1, 2, 3};
  std::vector<int> down(s.begin(), s.end());
  std::vector<int> up(s.rbegin(), s.rend());
  assert((down == std::vector<int>{1, 2, 3}));
  assert((up == std::vector<int>{3, 2, 1}));
}

/* Shuffling keeps the same cards, and every order of three cards is about
   equally likely. */

void
check_shuffle()
{
  stack s{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  s.shuffle();
  std::vector<int> cards(s.begin(), s.end());
  std::ranges::sort(cards);
  assert((cards == std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));

  constexpr int n = 600000;
  std::map<std::array<int, 3>, int> orders;
  for (int k = 0; k < n; ++k) {
    stack t{1, 2, 3};
    t.shuffle();
    ++orders[{t[0], t[1], t[2]}];
  }
  assert(orders.size() == 6);
  for (const auto& [order, count] : orders)
    assert(is_plausible(count, n, 1.0 / 6));

  stack one{1};
  one.shuffle();
  assert(equals(one, {1}));
  stack none;
  none.shuffle();
  assert(none.empty());
}

/* Equal seeds give equal shuffles. */

void
check_seeding()
{
  hap::seed(21);
  stack a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  a.shuffle();
  hap::seed(21);
  stack b{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  b.shuffle();
  assert(std::ranges::equal(a, b));
}

} /* namespace */

void
run_tests()
{
  check_construction();
  check_dealing();
  check_discarding();
  check_too_few();
  check_adding();
  check_iteration();
  check_shuffle();
  check_seeding();
}
