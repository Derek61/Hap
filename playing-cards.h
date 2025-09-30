/* hap/playing-cards.h -*- mode: c++ -*-
   Copyright (c) 2025 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_PLAYING_CARDS_H_
#define HAP_PLAYING_CARDS_H_

#include <hap/deck.h>

namespace hap {

class playing_card
{
public:
  enum Rank : unsigned char {
    Joker = 0, Two = 2, Three = 3, Four = 4, Five = 5, Six = 6, Seven = 7,
    Eight = 8, Nine = 9, Ten = 10, Jack = 11, Queen = 12, King = 13, Ace = 14
  };
  enum Suit : unsigned char { Bicycle, Diamonds, Clubs, Hearts, Spades };

  constexpr playing_card(Rank rank, Suit suit);

  constexpr Rank rank() const;
  constexpr Suit suit() const;

  constexpr std::strong_ordering operator<=>(const playing_card& card) const
    = default;

private:
  Rank r = Joker;
  Suit s = Bicycle;
};

using hand = stack<playing_card>;

deck<playing_card> playing_cards();

} /* namespace hap */

/* Implementation. */

constexpr
hap::playing_card::playing_card(Rank rank, Suit suit)
  : r{rank}, s{suit}
{
}

constexpr auto
hap::playing_card::rank() const -> Rank
{
  return r;
}

constexpr auto
hap::playing_card::suit() const -> Suit
{
  return s;
}

template<>
struct std::formatter<hap::playing_card::Suit> : std::formatter<std::string>
{
  auto format(hap::playing_card::Suit suit, auto& ctx) const
  {
    using enum hap::playing_card::Suit;

    auto&& out = ctx.out();
    switch (suit) {
    case Bicycle:  break;
    case Spades:   format_to(out, "♠︎"); break;
    case Hearts:   format_to(out, "♥︎"); break;
    case Clubs:    format_to(out, "♣︎"); break;
    case Diamonds: format_to(out, "♦︎"); break;
    }
    return out;
  }
};

template<>
struct std::formatter<hap::playing_card::Rank> : std::formatter<std::string>
{
  auto format(hap::playing_card::Rank rank, auto& ctx) const
  {
    using enum hap::playing_card::Rank;

    auto&& out = ctx.out();
    switch (rank) {
    case Joker: format_to(out, "Joker"); break;
    case Ace:   format_to(out, "A"); break;
    case King:  format_to(out, "K"); break;
    case Queen: format_to(out, "Q"); break;
    case Jack:  format_to(out, "J"); break;
    case Ten:   format_to(out, "T"); break;
    default: format_to(out, "{}", static_cast<unsigned int>(rank)); break;
    }
    return out;
  }
};

template<>
struct std::formatter<hap::playing_card> : std::formatter<std::string>
{
  auto format(hap::playing_card card, auto& ctx) const
  {
    using enum hap::playing_card::Suit;

    auto&& out = ctx.out();
    const auto suit = card.suit();
    if (suit == Hearts || suit == Diamonds) {
      static const char switch_to_red[] = "\x1B[38;5;124m";
      format_to(out, "{}", switch_to_red);
    }
    format_to(out, "{}{}", card.rank(), suit);
    if (suit == Hearts || suit == Diamonds) {
      static const char switch_to_black[] = "\x1B[0m";
      format_to(out, "{}", switch_to_black);
    }
    return out;
  }
};

#endif /* HAP_PLAYING_CARDS_H_ */
