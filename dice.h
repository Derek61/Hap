/* hap/dice.h -*- mode: c++ -*-
   Copyright (c) 2025 Derek B Clegg
   All rights reserved. */

#ifndef HAP_DICE_H_
#define HAP_DICE_H_

#include <hap/die.h>
#include <vector>

namespace hap {

class dice {
public:
  /* Create a set of dice. The number of sides is the default for a single
     die. */
  dice(unsigned int count);

  /* Create a set of n-sided dice, where `sides' specifies the number of
     sides. */
  dice(unsigned int count, unsigned int sides);

  /* Return the number of dice in this set. */
  unsigned int count() const;
  
  /* Roll all the dice and return the sum. */
  unsigned int roll();

  /* Roll the sum of the current state of the dice. */
  unsigned int sum() const;

  /* Iterate through all the dice in the set. */
  auto begin();
  auto end();

  auto begin() const;
  auto end() const;

private:
  unsigned int s = 0;
  std::vector<die> set;
};

} /* namespace hap */

/* Implementation. */

inline
hap::dice::dice(unsigned int count)
  : set{count, {}}
{
}

inline
hap::dice::dice(unsigned int count, unsigned int sides)
  : set{count, {sides}}
{
}

inline
unsigned int
hap::dice::count() const
{
  return static_cast<unsigned int>(set.size());
}

inline
unsigned int
hap::dice::roll()
{
  s = 0;
  for (auto& die : set)
    s += die.roll();
  return s;
}

inline
unsigned int
hap::dice::sum() const
{
  return s;
}

inline
auto
hap::dice::begin()
{
  return set.begin();
}

inline
auto
hap::dice::end()
{
  return set.end();
}

inline
auto
hap::dice::begin() const
{
  return set.cbegin();
}

inline
auto
hap::dice::end() const
{
  return set.cend();
}

#endif /* HAP_DICE_H_ */
