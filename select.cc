/* hap/select.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/select.h>
#include <hap/random.h>
#include <nyx/utility.h>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <unordered_set>
#include <utility>

std::vector<unsigned int>
hap::shuffle(unsigned int n)
{
  std::vector<unsigned int> elements(n);
  std::iota(elements.begin(), elements.end(), 1u);
  internal::shuffle(elements.begin(), elements.end());
  return elements;
}

/* Count the values in [n, m] in 64 bits, since there are 2^32 of them when
   `n' is 0 and `m' is the largest unsigned int. */

unsigned int
hap::select_one(unsigned int n, unsigned int m)
{
  NYX_PRECONDITION(n <= m);
  const uint64_t count = uint64_t{m} - n + 1;
  return n + static_cast<unsigned int>(internal::uniform(count));
}

/* When `k' is at least a quarter of `n', shuffle the first `k' places of
   the whole range (a partial Fisher-Yates shuffle), which needs space for
   at most 4 k elements. Otherwise use Floyd's algorithm, which needs space
   only for the `k' elements chosen. Floyd's algorithm chooses every set of
   `k' elements with equal probability, but not every order, so the chosen
   elements are then shuffled. */

std::vector<unsigned int>
hap::select_without_replacement(unsigned int k, unsigned int n)
{
  NYX_PRECONDITION(k <= n);

  if (k >= n / 4) {
    std::vector<unsigned int> elements(n);
    std::iota(elements.begin(), elements.end(), 1u);
    for (size_t i = 0; i < k; ++i) {
      const auto j = i + static_cast<size_t>(internal::uniform(n - i));
      std::swap(elements[i], elements[j]);
    }
    elements.resize(k);
    return elements;
  }

  /* For each j from n - k + 1 to n, choose t from [1, j]; take t if it
     hasn't been chosen yet, and j otherwise. */
  std::vector<unsigned int> chosen;
  chosen.reserve(k);
  std::unordered_set<unsigned int> seen;
  seen.reserve(k);
  for (unsigned int i = 0; i < k; ++i) {
    const unsigned int j = n - k + 1 + i;
    const auto t = select_one(1, j);
    const auto element = seen.contains(t) ? j : t;
    seen.insert(element);
    chosen.push_back(element);
  }
  internal::shuffle(chosen.begin(), chosen.end());
  return chosen;
}

std::vector<unsigned int>
hap::select_with_replacement(unsigned int k, unsigned int n)
{
  NYX_PRECONDITION(n > 0 || k == 0);

  std::vector<unsigned int> elements(k);
  for (auto& element : elements)
    element = select_one(1, n);
  return elements;
}
