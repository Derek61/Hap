/* deck.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/deck.h>
#include <hap/random.h>
#include <nyx/command-line.h>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <initializer_list>
#include <print>
#include <type_traits>
#include <utility>
#include <vector>

void run_tests();

int
main(int argc, char** argv)
{
  nyx::command_line cl{nyx::argcount::none};
  if (auto exit = cl.parse_command_line(argc, argv); exit)
    return *exit;
  std::println(stderr, "Checking decks of cards.");
  run_tests();
  return EXIT_SUCCESS;
}

namespace {

using deck = hap::deck<int>;

/* Return true if the undealt cards of `d' are `cards', top first. */

bool
equals(const deck& d, std::initializer_list<int> cards)
{
  return std::ranges::equal(d, cards);
}

/* A deck can be built from a vector, a list, or a range; the range
   constructor deduces the card type. The deduction needs parentheses:
   with braces, the list constructor makes a deck of iterators. */

void
check_construction()
{
  const std::vector<int> v{1, 2, 3};
  deck a{v};
  deck b{1, 2, 3};
  hap::deck c(v.begin(), v.end());
  static_assert(std::is_same_v<decltype(c), deck>);
  for (const deck* d : {&a, &b, &c}) {
    assert(d->size() == 3);
    assert(d->remaining_cards() == 3);
    assert(!d->empty());
    assert(equals(*d, {1, 2, 3}));
  }
}

/* Dealing and discarding reduce the cards remaining, but not the size of
   the deck. */

void
check_dealing()
{
  deck d{1, 2, 3, 4, 5, 6};
  assert(d.top() == 1);
  assert(d.deal_one_card() == 1);
  auto hand = d.deal(2);
  assert(std::ranges::equal(hand, std::vector<int>{2, 3}));
  d.discard(1);
  assert(equals(d, {5, 6}));
  assert(d.size() == 6);
  assert(d.remaining_cards() == 2);

  (void)d.deal(2);
  assert(d.empty());
  assert(d.size() == 6);
}

/* Cards can be returned to the bottom of the deck. */

void
check_adding()
{
  deck d{1, 2, 3, 4};
  auto hand = d.deal(2);
  d.add_to_bottom(9);
  assert(equals(d, {3, 4, 9}));
  d.add_to_bottom(hand);
  assert(equals(d, {3, 4, 9, 1, 2}));
  assert(std::ranges::equal(hand, std::vector<int>{1, 2}));
  d.add_to_bottom(std::move(hand));
  assert(equals(d, {3, 4, 9, 1, 2, 1, 2}));
  assert(hand.empty());
}

/* reset() restores the original cards in their original order, whatever
   was dealt or added. */

void
check_reset()
{
  deck d{1, 2, 3, 4};
  (void)d.deal(3);
  d.add_to_bottom(7);
  d.reset();
  assert(equals(d, {1, 2, 3, 4}));
  assert(d.remaining_cards() == d.size());
}

/* shuffle() restores every card, in some order. */

void
check_shuffle()
{
  deck d{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  (void)d.deal(4);
  d.shuffle();
  assert(d.remaining_cards() == 10);
  std::vector<int> cards(d.begin(), d.end());
  std::ranges::sort(cards);
  assert((cards == std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));

  hap::seed(8);
  d.shuffle();
  std::vector<int> first(d.begin(), d.end());
  hap::seed(8);
  d.shuffle();
  assert(std::ranges::equal(d, first));
}

/* A deck formats as its undealt cards separated by spaces; an empty deck
   formats as nothing. */

void
check_format()
{
  deck d{1, 2, 3};
  assert(std::format("{}", d) == "1 2 3");
  (void)d.deal_one_card();
  assert(std::format("{}", d) == "2 3");
  (void)d.deal(2);
  assert(std::format("{}", d).empty());
}

} /* namespace */

void
run_tests()
{
  check_construction();
  check_dealing();
  check_adding();
  check_reset();
  check_shuffle();
  check_format();
}
