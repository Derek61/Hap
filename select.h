/* hap/select.h -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_SELECT_H_
#define HAP_SELECT_H_

#include <vector>

namespace hap {

/* Return a shuffled set of elements from `1' to `n'. */
extern std::vector<unsigned int> shuffle(unsigned int n);

/* Select an element from `[n, m]'. `n' must not exceed `m'. */
extern unsigned int select_one(unsigned int n, unsigned int m);

/* Select `k' elements from `1' to `n' without replacement. The elements are
   in random order, so every ordered selection is equally likely. `k' must
   not exceed `n'. */
extern std::vector<unsigned int> select_without_replacement(unsigned int k,
  unsigned int n);

/* Select `k' elements from `1' to `n' with replacement. `n' must be
   positive unless `k' is 0. */
extern std::vector<unsigned int> select_with_replacement(unsigned int k,
  unsigned int n);

} /* namespace hap */

#endif /* HAP_SELECT_H_ */
