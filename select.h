/* hap/select.h -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_SELECT_H_
#define HAP_SELECT_H_

#include <vector>

namespace hap {

/* Return a shuffled set of elements from `1' to `N'. */
template<unsigned int N> std::array<unsigned int,N> shuffle();

/* Return a shuffled set of elements from `1' to `n'. */
std::vector<unsigned int> shuffle(unsigned int n);

/* Select an element from `[n, m]'. `n' must not exceed `m'. */
unsigned int select_one(unsigned int n, unsigned int m);

/* Select `k' elements from `1' to `n' without replacement. The elements are
   in random order, so every ordered selection is equally likely. `k' must
   not exceed `n'. */
std::vector<unsigned int> select_without_replacement(unsigned int k,
  unsigned int n);

/* Select `k' elements from `1' to `n' with replacement. */
std::vector<unsigned int> select_with_replacement(unsigned int k,
  unsigned int n);

} /* namespace hap */

/* Implementation. */

#include <numeric>
#include <hap/random.h>

template<unsigned int N>
std::array<unsigned int,N>
hap::shuffle()
{
  std::array<unsigned int,N> elements;
  std::iota(elements.begin(), elements.end(), 1u);
  internal::shuffle(elements.begin(), elements.end());
  return elements;
}

#endif /* HAP_SELECT_H_ */
