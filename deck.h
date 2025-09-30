/* hap/deck.h -*- mode: c++ -*-
   Copyright (c) 2025 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_DECK_H_
#define HAP_DECK_H_

#include <hap/stack.h>
#include <vector>

namespace hap {

template<typename Card>
class deck
{
public:
  deck(const std::vector<Card>& cards);
  deck(const std::initializer_list<Card>& cards);
  template<typename Iterator> deck(Iterator begin, Iterator end);

  /* Return the number of cards in the entire deck. */
  size_t size() const;

  /* Return true if all the cards in the deck have been dealt; false
     otherwise. */
  bool empty() const;

  /* Return the number of undealt cards remaining in the deck. */
  size_t remaining_cards() const;

  /* Look at the card on the top of the deck. */
  Card top() const;

  /* Deal the card from the top of the deck, reducing the number of cards in
     the deck by 1. */
  Card deal_one_card();

  /* Deal `n' cards from the top of the deck, reducing the number of cards
     in the deck by `n'. */
  stack<Card> deal(size_t n);

  /* Discard `n' cards from the top of the deck. */
  void discard(size_t n);

  /* Add a card to the bottom of the stack. */
  void add_to_bottom(Card c);

  /* Add a stack of cards to the bottom of the stack. */
  void add_to_bottom(stack<Card>&& stack);
  void add_to_bottom(const stack<Card>& stack);

  /* Return the deck to its original state. */
  void reset();

  /* Return the deck to its original state and shuffle it. */
  void shuffle();

  /* Iterate through the cards remaining in the deck. */
  auto begin() const;
  auto end() const;

private:
  const std::vector<Card> cards;	// The set of all cards.
  stack<Card> stack = { cards.begin(), cards.end() };
};

template<typename Iterator> deck(Iterator, Iterator) ->
  deck<typename std::iterator_traits<Iterator>::value_type>;
  
} /* namespace hap */

/* Implementation. */

#include <format>

template<typename Card>
hap::deck<Card>::deck(const std::vector<Card>& cards)
  : deck{cards.begin(), cards.end()}
{
}

template<typename Card>
hap::deck<Card>::deck(const std::initializer_list<Card>& cards)
  : deck{cards.begin(), cards.end()}
{
}

template<typename Card>
template<typename Iterator>
hap::deck<Card>::deck(Iterator begin, Iterator end)
  : cards{begin, end}
{
}

template<typename Card>
size_t
hap::deck<Card>::size() const
{
  return cards.size();
}

template<typename Card>
bool
hap::deck<Card>::empty() const
{
  return stack.empty();
}

template<typename Card>
size_t
hap::deck<Card>::remaining_cards() const
{
  return stack.count();
}

template<typename Card>
Card
hap::deck<Card>::top() const
{
  return stack.top();
}

template<typename Card>
Card
hap::deck<Card>::deal_one_card()
{
  return stack.deal_one_card();
}

template<typename Card>
hap::stack<Card>
hap::deck<Card>::deal(size_t n)
{
  return stack.deal(n);
}

template<typename Card>
void
hap::deck<Card>::discard(size_t n)
{
  stack.discard(n);
}

template<typename Card>
void
hap::deck<Card>::add_to_bottom(Card c)
{
  stack.add_to_bottom(c);
}

template<typename Card>
void
hap::deck<Card>::add_to_bottom(hap::stack<Card>&& s)
{
  stack.add_to_bottom(std::move(s));
}

template<typename Card>
void
hap::deck<Card>::add_to_bottom(const hap::stack<Card>& s)
{
  stack.add_to_bottom(s);
}

template<typename Card>
void
hap::deck<Card>::reset()
{
  stack = { cards.begin(), cards.end() };
}

template<typename Card>
void
hap::deck<Card>::shuffle()
{
  reset();
  stack.shuffle();
}

template<typename Card>
auto
hap::deck<Card>::begin() const
{
  return stack.begin();
}

template<typename Card>
auto
hap::deck<Card>::end() const
{
  return stack.end();
}

template<typename Card>
struct std::formatter<hap::deck<Card>> : std::formatter<std::string>
{
  auto format(const hap::deck<Card>& deck, auto& ctx) const
  {
    auto&& out = ctx.out();
    if (deck.size() > 0) {
      auto t = deck.begin();
      format_to(out, "{}", *t++);
      while (t != deck.end())
	format_to(out, " {}", *t++);
    }
    return out;
  }
};

#endif /* HAP_DECK_H_ */
