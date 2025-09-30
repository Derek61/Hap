/* hap/coin.h -*- mode: c++ -*-
   Copyright (c) 2025 Derek B Clegg
   All rights reserved. */

#ifndef HAP_COIN_H_
#define HAP_COIN_H_

#include <hap/generator.h>

namespace hap {

class coin {
public:
  enum Face : unsigned char { Heads, Tails };

  coin() = default;

  Face toss();

  bool heads() const;
  bool tails() const;

private:
  Face face = Heads;
  std::uniform_int_distribution<unsigned char> distribution{Heads, Tails};
};

} /* namespace hap */

inline auto
hap::coin::toss() -> Face
{
  face = static_cast<Face>(distribution(generator));
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
