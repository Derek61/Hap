/* hap/coin.h -*- mode: c++ -*-
   Copyright (c) 2025 Derek B Clegg
   All rights reserved. */

#ifndef HAP_COIN_H_
#define HAP_COIN_H_

#if HAP_USE_GENERATOR
#include <hap/generator.h>
#else
#include <nyx/uniform-boolean.h>
#endif

namespace hap {

class coin
{
public:
  enum Face : unsigned char { Heads, Tails };

  coin() = default;

  Face toss();

  bool heads() const;
  bool tails() const;

private:
  Face face = Heads;
#if HAP_USE_GENERATOR
  std::uniform_int_distribution<unsigned char> distribution{Heads, Tails};
#else
  nyx::uniform_boolean b;
#endif
};

} /* namespace hap */

/* Implementation. */

inline auto
hap::coin::toss() -> Face
{
#if HAP_USE_GENERATOR
  face = static_cast<Face>(distribution(generator));
#else
  face = b() ? Heads : Tails;
#endif
  return face;
}

inline bool
hap::coin::heads() const
{
  return face == Heads;
}

inline bool
hap::coin::tails() const
{
  return face == Tails;
}

#endif /* HAP_COIN_H_ */
