/* hap/playing-cards.cc -*- mode: c++ -*-
   Copyright (c) 2025 Derek B. Clegg
   All rights reserved. */

#include <hap/playing-cards.h>

namespace hap {

namespace {

constexpr auto
R(unsigned int r)
{
  using enum playing_card::Rank;
  switch (r) {
  case 2: return Two;
  case 3: return Three;
  case 4: return Four;
  case 5: return Five;
  case 6: return Six;
  case 7: return Seven;
  case 8: return Eight;
  case 9: return Nine;
  case 10: return Ten;
  case 11: return Jack;
  case 12: return Queen;
  case 13: return King;
  case 14: return Ace;
  default: abort();
  }
}

} /* namespace */

deck<playing_card>
playing_cards()
{
  using enum playing_card::Suit;
  enum { T = 10, J = 11, Q = 12, K = 13, A = 14 };
  return {
    {R(2), Diamonds}, {R(2), Clubs}, {R(2), Hearts}, {R(2), Spades},
    {R(3), Diamonds}, {R(3), Clubs}, {R(3), Hearts}, {R(3), Spades},
    {R(4), Diamonds}, {R(4), Clubs}, {R(4), Hearts}, {R(4), Spades},
    {R(5), Diamonds}, {R(5), Clubs}, {R(5), Hearts}, {R(5), Spades},
    {R(6), Diamonds}, {R(6), Clubs}, {R(6), Hearts}, {R(6), Spades},
    {R(7), Diamonds}, {R(7), Clubs}, {R(7), Hearts}, {R(7), Spades},
    {R(8), Diamonds}, {R(8), Clubs}, {R(8), Hearts}, {R(8), Spades},
    {R(9), Diamonds}, {R(9), Clubs}, {R(9), Hearts}, {R(9), Spades},
    {R(T), Diamonds}, {R(T), Clubs}, {R(T), Hearts}, {R(T), Spades},
    {R(J), Diamonds}, {R(J), Clubs}, {R(J), Hearts}, {R(J), Spades},
    {R(Q), Diamonds}, {R(Q), Clubs}, {R(Q), Hearts}, {R(Q), Spades},
    {R(K), Diamonds}, {R(K), Clubs}, {R(K), Hearts}, {R(K), Spades},
    {R(A), Diamonds}, {R(A), Clubs}, {R(A), Hearts}, {R(A), Spades}
  };
}

} /* namespace hap */
