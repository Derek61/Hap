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
  /* Create a 6-sided die and roll it. */
  die() = default;

  /* Create an `n'-sided die and roll it. `n' must be positive. */
  explicit die(unsigned int n);

  /* Create an `n'-sided die showing `value', without rolling it. `value'
     must be in [1, n]. */
  die(unsigned int n, unsigned int value);

  /* Return the number of sides of this die. */
  unsigned int sides() const;

  /* Roll the die and return its value. */
  unsigned int roll();

  /* Return the die's value. */
  unsigned int value() const;

private:
  unsigned int sides_ = 6;	/* Number of sides. */
  unsigned int value_ = roll(); /* Value showing, in [1, sides_]. */
};

} /* namespace hap */

/* Implementation. */

inline
hap::die::die(unsigned int n)
  : sides_{n}
{
  NYX_ASSERT(n > 0);
}

inline
hap::die::die(unsigned int n, unsigned int value)
  : sides_{n}, value_{value}
{
  NYX_ASSERT(n > 0);
  NYX_ASSERT(value >= 1 && value <= n);
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
