/* playing-cards.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/playing-cards.h>
#include <nyx/command-line.h>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <print>
#include <set>
#include <string>
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
  std::println(stderr, "Checking playing cards.");
  run_tests();
  return EXIT_SUCCESS;
}

namespace {

using hap::playing_card;
using enum playing_card::Rank;
using enum playing_card::Suit;

/* Cards compare by rank, then by suit. */

constexpr playing_card two_of_spades{Two, Spades};
constexpr playing_card king_of_hearts{King, Hearts};
constexpr playing_card ace_of_diamonds{Ace, Diamonds};
constexpr playing_card ace_of_hearts{Ace, Hearts};
constexpr playing_card ace_of_spades{Ace, Spades};

static_assert(two_of_spades < king_of_hearts);
static_assert(king_of_hearts < ace_of_diamonds);
static_assert(ace_of_diamonds < ace_of_hearts);
static_assert(ace_of_hearts < ace_of_spades);
static_assert(ace_of_spades == playing_card{Ace, Spades});
static_assert(ace_of_spades != ace_of_hearts);
static_assert(ace_of_spades.rank() == Ace && ace_of_spades.suit() == Spades);

static_assert(std::is_same_v<hap::hand, hap::stack<playing_card>>);

/* The standard deck has 52 different cards: every rank from two to ace in
   each of the four suits, in increasing order. */

void
check_deck()
{
  auto d = hap::playing_cards();
  assert(d.size() == 52);
  assert(d.remaining_cards() == 52);

  const std::vector<playing_card> cards(d.begin(), d.end());
  assert(std::ranges::adjacent_find(cards, std::ranges::greater_equal{})
	 == cards.end());

  std::set<std::pair<int, int>> seen;
  for (auto card : cards) {
    assert(card.rank() >= Two && card.rank() <= Ace);
    assert(card.suit() != Bicycle);
    seen.insert({card.rank(), card.suit()});
  }
  assert(seen.size() == 52);

  assert(cards.front() == (playing_card{Two, Diamonds}));
  assert(cards.back() == ace_of_spades);
}

/* Cards format as rank then suit symbol, with hearts and diamonds in
   red. */

void
check_format()
{
  const std::string red = "\x1B[38;5;124m";
  const std::string black = "\x1B[0m";

  assert(std::format("{}", ace_of_spades) == "A♠︎");
  assert(std::format("{}", playing_card{Ten, Clubs}) == "T♣︎");
  assert(std::format("{}", playing_card{Jack, Spades}) == "J♠︎");
  assert(std::format("{}", playing_card{Queen, Clubs}) == "Q♣︎");
  assert(std::format("{}", two_of_spades) == "2♠︎");
  assert(std::format("{}", playing_card{Nine, Clubs}) == "9♣︎");
  assert(std::format("{}", king_of_hearts) == red + "K♥︎" + black);
  assert(std::format("{}", ace_of_diamonds) == red + "A♦︎" + black);
  assert(std::format("{}", playing_card{Joker, Bicycle}) == "Joker");
}

/* A shuffled deck still holds the same 52 cards. */

void
check_shuffle()
{
  auto d = hap::playing_cards();
  const std::vector<playing_card> before(d.begin(), d.end());
  d.shuffle();
  std::vector<playing_card> after(d.begin(), d.end());
  assert(after != before);
  std::ranges::sort(after);
  assert(after == before);
}

} /* namespace */

void
run_tests()
{
  check_deck();
  check_format();
  check_shuffle();
}
