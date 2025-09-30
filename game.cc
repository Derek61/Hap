/* game.cc -*- mode: c++ -*-
   Copyright (c) 2025 Derek B. Clegg
   All rights reserved. */

#include <nyx/error.h>
#include <nyx/command-line.h>
#include <cassert>
#include <iostream>
#include "playing-cards.h"

using namespace nyx;
using PlayingCard = hap::playing_card;

constexpr PlayingCard King_of_Hearts =
  { PlayingCard::King, PlayingCard::Hearts };
constexpr PlayingCard Ace_of_Hearts =
  { PlayingCard::Ace, PlayingCard::Hearts };
constexpr PlayingCard Ace_of_Spades =
  { PlayingCard::Ace, PlayingCard::Spades };

static_assert(King_of_Hearts != Ace_of_Spades);
static_assert(King_of_Hearts < Ace_of_Spades);
static_assert(King_of_Hearts <= Ace_of_Spades);
static_assert(Ace_of_Spades > King_of_Hearts);
static_assert(Ace_of_Spades >= King_of_Hearts);

static_assert(Ace_of_Hearts != Ace_of_Spades);
static_assert(Ace_of_Hearts < Ace_of_Spades);
static_assert(Ace_of_Hearts <= Ace_of_Spades);
static_assert(Ace_of_Spades > Ace_of_Hearts);
static_assert(Ace_of_Spades >= Ace_of_Hearts);

static_assert(Ace_of_Hearts == Ace_of_Hearts);

int
main(int argc, char** argv)
{
  nyx::command_line cl{nyx::argcount::none};
  if (auto exit = cl.parse_command_line(argc, argv); exit)
    return *exit;

  auto deck = hap::playing_cards();
  assert(deck.size() == 52);
  {
    std::print("Deal 1 card:\n");
    auto card = deck.deal_one_card();
    std::print("{}\n", card);
  }
  {
    std::print("Deal 7 cards:\n");
    auto hand = deck.deal(7);
    std::print("{}\n", hand);
    std::print("Discard top 3 cards from this hand:\n");
    hand.discard(3);
    std::print("{}\n", hand);
  }
  {
    std::print("Deal 7 more cards:\n");
    auto hand = deck.deal(7);
    std::print("{}\n", hand);
  }
  {
    std::print("Deal 7 more cards:\n");
    auto hand = deck.deal(7);
    std::print("{}\n", hand);
    std::print("Deal 3 cards from this hand:\n");
    auto h = hand.deal(3);
    std::print("{} -- {}\n", h, hand);
    std::print("Add the 3 cards to the top of this hand:\n");
    hand.add_to_top(std::move(h));
    std::print("{} -- {}\n", h, hand);
  }
  {
    std::print("Deal 7 more cards:\n");
    auto hand = deck.deal(7);
    std::print("{}\n", hand);
    std::print("Deal 3 cards from this hand:\n");
    auto h = hand.deal(3);
    std::print("{} -- {}\n", h, hand);
    std::print("Add the 3 cards to the bottom of this hand:\n");
    hand.add_to_bottom(std::move(h));
    std::print("{} -- {}\n", h, hand);
  }
  std::print("Remaining cards:\n");
  for (auto card : deck)
    std::print("{} ", card);
  std::print("\n");

  deck.shuffle();
  assert(deck.remaining_cards() == 52);

  return EXIT_SUCCESS;
}
