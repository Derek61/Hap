/* hap/stack.h -*- mode: c++ -*-
   Copyright (c) 2025 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_STACK_H_
#define HAP_STACK_H_

/* A stack is a collection of some number of cards. The number of cards may
   change as cards are drawn or discarded. */

#include <deque>

namespace hap {

template<typename Card>
class stack
{
public:
  stack() = default;
  stack(const std::initializer_list<Card>& cards);
  template<typename Iterator> stack(Iterator begin, Iterator end);

  /* Return true if the stack is empty; false otherwise. */
  bool empty() const;

  /* Return the number of cards in the stack. */
  size_t count() const;

  /* Look at the card on the top of the stack. */
  Card top() const;

  /* Deal the card from the top of the stack, reducing the number of cards
     in the stack by 1. */
  Card deal_one_card();

  /* Deal `n' cards from the top of the stack, reducing the number of cards
     in the stack by `n'. */
  stack deal(size_t n);

  /* Discard `n' cards from the top of the stack. */
  void discard(size_t n);

  /* Discard all cards from the stack. */
  void discard_all();

  /* Add a card to the top of the stack. */
  void add_to_top(Card c);

  /* Add a stack of cards to the top of the stack. */
  void add_to_top(stack&& stack);
  void add_to_top(const stack& stack);

  /* Add a card to the bottom of the stack. */
  void add_to_bottom(Card c);

  /* Add a stack of cards to the bottom of the stack. */
  void add_to_bottom(stack&& stack);
  void add_to_bottom(const stack& stack);

  /* Return the card at position `k'. The top of the stack is 0. */
  Card operator[](size_t k) const;
  
  /* Shuffle the cards in the stack. */
  void shuffle();

  /* Iterate through the cards in the stack top to bottom. */
  auto begin() const;
  auto end() const;
  
  /* Iterate through the cards in the stack from bottom to top. */
  auto rbegin() const;
  auto rend() const;

private:
  std::deque<Card> cards;
};

} /* namespace hap */

/* Implementation. */

#include <hap/generator.h>
#include <nyx/exception.h>
#include <algorithm>
#include <iterator>

template<typename Card>
hap::stack<Card>::stack(const std::initializer_list<Card>& cards)
  : stack{cards.begin(), cards.end()}
{
}

template<typename Card>
template<typename Iterator>
hap::stack<Card>::stack(Iterator begin, Iterator end)
  : cards{begin, end}
{
}

template<typename Card>
bool
hap::stack<Card>::empty() const
{
  return cards.empty();
}

template<typename Card>
size_t
hap::stack<Card>::count() const
{
  return cards.size();
}

template<typename Card>
Card
hap::stack<Card>::top() const
{
  if (cards.empty())
    throw nyx::exception("not enough cards");
  return cards.front();
}

template<typename Card>
Card
hap::stack<Card>::deal_one_card()
{
  auto card = top();
  cards.pop_front();
  return card;
}

template<typename Card>
hap::stack<Card>
hap::stack<Card>::deal(size_t n)
{
  if (n > count())
    throw nyx::exception("not enough cards");
  auto t = cards.begin();
  using diff_t = typename std::deque<Card>::difference_type;
  auto e = std::next(t, static_cast<diff_t>(n));
  stack<Card> hand{t, e};
  cards.erase(t, e);
  return hand;
}

template<typename Card>
void
hap::stack<Card>::discard(size_t n)
{
  if (n > count())
    throw nyx::exception("not enough cards");
  auto t = cards.begin();
  using diff_t = typename std::deque<Card>::difference_type;
  auto e = std::next(t, static_cast<diff_t>(n));
  cards.erase(t, e);
}

template<typename Card>
void
hap::stack<Card>::discard_all()
{
  cards.clear();
}

template<typename Card>
void
hap::stack<Card>::add_to_top(Card c)
{
  cards.push_front(c);
}

template<typename Card>
void
hap::stack<Card>::add_to_top(stack&& stack)
{
  std::move(stack.rbegin(), stack.rend(), std::front_inserter(cards));
  stack.discard_all();
}

template<typename Card>
void
hap::stack<Card>::add_to_top(const stack& stack)
{
  std::copy(stack.rbegin(), stack.rend(), std::front_inserter(cards));
}

template<typename Card>
void
hap::stack<Card>::add_to_bottom(Card c)
{
  cards.push_back(c);
}

template<typename Card>
void
hap::stack<Card>::add_to_bottom(stack&& stack)
{
  std::move(stack.begin(), stack.end(), std::back_inserter(cards));
  stack.discard_all();
}

template<typename Card>
void
hap::stack<Card>::add_to_bottom(const stack& stack)
{
  std::copy(stack.begin(), stack.end(), std::back_inserter(cards));
}

template<typename Card>
Card
hap::stack<Card>::operator[](size_t index) const
{
  if (index >= count())
    throw nyx::exception("not enough cards");
  return cards[index];
}

template<typename Card>
void
hap::stack<Card>::shuffle()
{
  std::shuffle(cards.begin(), cards.end(), generator);
}

template<typename Card>
auto
hap::stack<Card>::begin() const
{
  return cards.cbegin();
}

template<typename Card>
auto
hap::stack<Card>::end() const
{
  return cards.cend();
}

template<typename Card>
auto
hap::stack<Card>::rbegin() const
{
  return cards.crbegin();
}

template<typename Card>
auto
hap::stack<Card>::rend() const
{
  return cards.crend();
}

#endif /* HAP_STACK_H_ */
