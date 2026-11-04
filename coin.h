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

  /* Create a coin and toss it. */
  coin();

  /* Create a coin showing `face'. */
  explicit coin(Face face);

  /* Toss the coin and return the face that's up. */
  Face toss();

  /* Return the face of the coin that's up. */
  Face face() const;

  /* Return true if the coin shows heads; false otherwise. */
  bool heads() const;

  /* Return true if the coin shows tails; false otherwise. */
  bool tails() const;

private:
  Face face_;			/* The face that's up. */
};

} /* namespace hap */

/* Implementation. */

inline
hap::coin::coin()
{
  toss();
}

inline
hap::coin::coin(Face face)
  : face_{face}
{
}

inline auto
hap::coin::toss() -> Face
{
  face_ = internal::next_bit() ? Face::Heads : Face::Tails;
  return face_;
}

inline auto
hap::coin::face() const -> Face
{
  return face_;
}

inline bool
hap::coin::heads() const
{
  return face_ == Face::Heads;
}

inline bool
hap::coin::tails() const
{
  return face_ == Face::Tails;
}

#endif /* HAP_COIN_H_ */
