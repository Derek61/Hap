/* hap/dice.h -*- mode: c++ -*-
   Copyright (c) 2025-2026 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_DICE_H_
#define HAP_DICE_H_

#include <hap/die.h>
#include <nyx/static-vector.h>

namespace hap {

/* A set of dice. */

class dice
{
public:
  /* The largest number of dice in a set. */
  static constexpr size_t capacity = 32;

  /* Create a set of `count' 6-sided dice. Throw `std::bad_alloc' if `count'
     exceeds `capacity'. */
  explicit dice(size_t count);

  /* Create a set of `count' dice, each with `sides' sides. Throw
     `std::bad_alloc' if `count' exceeds `capacity'. */
  dice(size_t count, unsigned int sides);

  /* Return the number of dice in this set. */
  size_t count() const;

  /* Roll all the dice and return the sum. */
  unsigned int roll();

  /* Return the sum of the dice's current values. */
  unsigned int sum() const;

  /* Return the die with the lowest value. If several dice share the same
     value, return the first one. */
  die& lowest();
  const die& lowest() const;

  /* Return the die with the highest value. If several dice share the same
     value, return the first one. */
  die& highest();
  const die& highest() const;

  /* Return die `k'. */
  die& operator[](size_t k);
  const die& operator[](size_t k) const;

  /* Iterate through all the dice in the set. */
  auto begin();
  auto end();
  auto begin() const;
  auto end() const;

private:
  nyx::static_vector<die, capacity> set_;
};

} /* namespace hap */

/* Implementation. */

#include <nyx/utility.h>
#include <algorithm>

inline
hap::dice::dice(size_t count)
  : set_(count)
{
}

inline
hap::dice::dice(size_t count, unsigned int sides)
  : set_(count, die{sides})
{
}

inline size_t
hap::dice::count() const
{
  return set_.size();
}

inline unsigned int
hap::dice::roll()
{
  for (auto& d : set_)
    d.roll();
  return sum();
}

inline unsigned int
hap::dice::sum() const
{
  unsigned int s = 0;
  for (const auto& d : set_)
    s += d.value();
  return s;
}

/* std::ranges::min_element returns the first of equal elements. */

inline auto
hap::dice::lowest() -> die&
{
  NYX_PRECONDITION(!set_.empty());
  return *std::ranges::min_element(set_, {}, &die::value);
}

inline auto
hap::dice::lowest() const -> const die&
{
  NYX_PRECONDITION(!set_.empty());
  return *std::ranges::min_element(set_, {}, &die::value);
}

/* std::ranges::max_element returns the first of equal elements. */

inline auto
hap::dice::highest() -> die&
{
  NYX_PRECONDITION(!set_.empty());
  return *std::ranges::max_element(set_, {}, &die::value);
}

inline auto
hap::dice::highest() const -> const die&
{
  NYX_PRECONDITION(!set_.empty());
  return *std::ranges::max_element(set_, {}, &die::value);
}

inline auto
hap::dice::operator[](size_t k) -> die&
{
  NYX_PRECONDITION(k < set_.size());
  return set_[k];
}

inline auto
hap::dice::operator[](size_t k) const -> const die&
{
  NYX_PRECONDITION(k < set_.size());
  return set_[k];
}

inline auto
hap::dice::begin()
{
  return set_.begin();
}

inline auto
hap::dice::end()
{
  return set_.end();
}

inline auto
hap::dice::begin() const
{
  return set_.begin();
}

inline auto
hap::dice::end() const
{
  return set_.end();
}

#endif /* HAP_DICE_H_ */
