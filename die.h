/* hap/die.h -*- mode: c++ -*-
   Copyright (c) 2025 Derek B Clegg
   All rights reserved. */

#ifndef HAP_DIE_H_
#define HAP_DIE_H_

#if HAP_USE_GENERATOR
#include <hap/generator.h>
#else
#include <nyx/uniform-integer.h>
#endif

namespace hap {

class die
{
public:
  /* Create a 6-sided die. */
  die() = default;

  /* Create an n-sided die. */
  die(unsigned int sides);

  /* Return the number of sides for this die. */
  unsigned int sides() const;

  /* Roll the die and return its value. */
  unsigned int roll();

  /* Roll the current value of the die. */
  unsigned int value() const;

private:
  unsigned int v = 0;
  const unsigned int s = 6;
#if HAP_USE_GENERATOR
  std::uniform_int_distribution<unsigned int> distribution{1, s};
#else
  nyx::uniform_integer<unsigned int> d{1, 6};
#endif
};

} /* namespace hap */

/* Implementation. */

inline
hap::die::die(unsigned int sides)
  : s{sides}
{
}

inline
unsigned int
hap::die::sides() const
{
  return s;
}

inline
unsigned int
hap::die::roll()
{
#if HAP_USE_GENERATOR
  v = distribution(generator);
#else
  v = d();
#endif
  return v;
}

inline
unsigned int
hap::die::value() const
{
  return v;
}

#endif /* HAP_DIE_H_ */
