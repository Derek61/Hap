/* hap/die.h -*- mode: c++ -*-
   Copyright (c) 2025-2026 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_DIE_H_
#define HAP_DIE_H_

#include <hap/random.h>
#include <nyx/utility.h>

namespace hap {

class die
{
public:
  /* Create a 6-sided die. */
  die() = default;

  /* Create an `n'-sided die. `n' must be positive. */
  explicit die(unsigned int n);

  /* Return the number of sides of this die. */
  unsigned int sides() const;

  /* Roll the die and return its value. */
  unsigned int roll();

  /* Return the value of the last roll, or 0 if the die hasn't been
     rolled. */
  unsigned int value() const;

private:
  unsigned int sides_ = 6;		/* Number of sides. */
  unsigned int value_ = 0;		/* Value of the last roll. */
};

} /* namespace hap */

/* Implementation. */

inline
hap::die::die(unsigned int n)
  : sides_{n}
{
  NYX_ASSERT(n > 0);
}

inline unsigned int
hap::die::sides() const
{
  return sides_;
}

inline unsigned int
hap::die::roll()
{
  value_ = 1 + static_cast<unsigned int>(internal::uniform(sides_));
  return value_;
}

inline unsigned int
hap::die::value() const
{
  return value_;
}

#endif /* HAP_DIE_H_ */
