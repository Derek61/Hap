/* hap/stack.h -*- mode: c++ -*-
   Copyright (c) 2025-2026 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_STACK_H_
#define HAP_STACK_H_

/* A stack is a collection of some number of cards. The number of cards may
   change as cards are drawn or discarded. */

#include <cstddef>
#include <deque>
#include <initializer_list>

namespace hap {

template<typename Card>
class stack
{
public:
  stack() = default;
  stack(const std::initializer_list<Card>& cards);
  template<std::input_iterator Iterator> stack(Iterator begin, Iterator end);

  /* Return true if the stack is empty; false otherwise. */
  bool empty() const;

  /* Return the number of cards in the stack. */
  size_t count() const;

  /* Peek at the card on the top of the stack. */
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
  void add_to_top(Card card);

  /* Add a stack of cards to the top of the stack. */
  void add_to_top(stack&& cards);
  void add_to_top(const stack& cards);

  /* Add a card to the bottom of the stack. */
  void add_to_bottom(Card card);

  /* Add a stack of cards to the bottom of the stack. */
  void add_to_bottom(stack&& cards);
  void add_to_bottom(const stack& cards);

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
  std::deque<Card> cards_;
};

template<std::input_iterator Iterator> stack(Iterator, Iterator) ->
  stack<typename std::iterator_traits<Iterator>::value_type>;

} /* namespace hap */

/* Implementation. */

#include <hap/random.h>
#include <nyx/exception.h>
#include <algorithm>
#include <iterator>

template<typename Card>
hap::stack<Card>::stack(const std::initializer_list<Card>& s)
  : stack{s.begin(), s.end()}
{
}

template<typename Card>
template<std::input_iterator Iterator>
hap::stack<Card>::stack(Iterator begin, Iterator end)
  : cards_{begin, end}
{
}

template<typename Card>
bool
hap::stack<Card>::empty() const
{
  return cards_.empty();
}

template<typename Card>
size_t
hap::stack<Card>::count() const
{
  return cards_.size();
}

template<typename Card>
Card
hap::stack<Card>::top() const
{
  if (cards_.empty())
    throw nyx::exception("not enough cards");
  return cards_.front();
}

template<typename Card>
Card
hap::stack<Card>::deal_one_card()
{
  auto card = top();
  cards_.pop_front();
  return card;
}

template<typename Card>
hap::stack<Card>
hap::stack<Card>::deal(size_t n)
{
  if (n > count())
    throw nyx::exception("not enough cards");

  using diff_t = typename std::deque<Card>::difference_type;
  auto t = cards_.begin();
  auto e = std::next(t, static_cast<diff_t>(n));
  stack<Card> hand{t, e};
  cards_.erase(t, e);
  return hand;
}

template<typename Card>
void
hap::stack<Card>::discard(size_t n)
{
  if (n > count())
    throw nyx::exception("not enough cards");

  using diff_t = typename std::deque<Card>::difference_type;
  auto t = cards_.begin();
  auto e = std::next(t, static_cast<diff_t>(n));
  cards_.erase(t, e);
}

template<typename Card>
void
hap::stack<Card>::discard_all()
{
  cards_.clear();
}

template<typename Card>
void
hap::stack<Card>::add_to_top(Card card)
{
  cards_.push_front(card);
}

template<typename Card>
void
hap::stack<Card>::add_to_top(stack&& s)
{
  std::move(s.rbegin(), s.rend(), std::front_inserter(cards_));
  s.discard_all();
}

template<typename Card>
void
hap::stack<Card>::add_to_top(const stack& s)
{
  std::copy(s.rbegin(), s.rend(), std::front_inserter(cards_));
}

template<typename Card>
void
hap::stack<Card>::add_to_bottom(Card card)
{
  cards_.push_back(card);
}

template<typename Card>
void
hap::stack<Card>::add_to_bottom(stack&& s)
{
  std::move(s.begin(), s.end(), std::back_inserter(cards_));
  s.discard_all();
}

template<typename Card>
void
hap::stack<Card>::add_to_bottom(const stack& s)
{
  std::copy(s.begin(), s.end(), std::back_inserter(cards_));
}

template<typename Card>
Card
hap::stack<Card>::operator[](size_t index) const
{
  if (index >= count())
    throw nyx::exception("not enough cards");
  return cards_[index];
}

template<typename Card>
void
hap::stack<Card>::shuffle()
{
  internal::shuffle(cards_.begin(), cards_.end());
}

template<typename Card>
auto
hap::stack<Card>::begin() const
{
  return cards_.cbegin();
}

template<typename Card>
auto
hap::stack<Card>::end() const
{
  return cards_.cend();
}

template<typename Card>
auto
hap::stack<Card>::rbegin() const
{
  return cards_.crbegin();
}

template<typename Card>
auto
hap::stack<Card>::rend() const
{
  return cards_.crend();
}

#endif /* HAP_STACK_H_ */
