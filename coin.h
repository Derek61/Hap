/* hap/coin.h -*- mode: c++ -*-
   Copyright (c) 2025-2026 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_COIN_H_
#define HAP_COIN_H_

#include <hap/random.h>

namespace hap {

class coin
{
public:
  enum Face : unsigned char { Heads, Tails };

  /* Create a coin showing heads. */
  coin() = default;

  /* Toss the coin and return the face that's up. */
  Face toss();

  /* Return the face showing after a flip. */
  Face face() const;

  /* Return true if the coin shows heads; false otherwise. */
  bool heads() const;

  /* Return true if the coin shows tails; false otherwise. */
  bool tails() const;

private:
  Face f = Face::Heads;
};

} /* namespace hap */

/* Implementation. */

inline auto
hap::coin::toss() -> Face
{
  f = internal::next_bit() ? Face::Heads : Face::Tails;
  return f;
}

inline auto
hap::coin::face() const -> Face
{
  return f;
}

inline bool
hap::coin::heads() const
{
  return f == Face::Heads;
}

inline bool
hap::coin::tails() const
{
  return f == Face::Tails;
}

#endif /* HAP_COIN_H_ */
