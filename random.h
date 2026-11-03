/* hap/random.h -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_RANDOM_H_
#define HAP_RANDOM_H_

/* The source of randomness for the library. Each thread has its own small
   engine, which every object on that thread draws from; no object holds an
   engine of its own. Every engine is derived from a single global seed, so
   setting the seed makes a run repeatable. */

#include <array>
#include <cstdint>
#include <iterator>

namespace hap {

/* Set the global seed. Each thread's engine restarts from the new seed
   before its next draw. Set the seed before starting any threads that draw
   from Hap. */
void seed(uint64_t s);

/* Return the global seed. If none has been set, one is chosen from
   std::random_device the first time it is needed. */
uint64_t seed();

namespace internal {

/* A small, fast engine (xoshiro256**) meeting the requirements of a uniform
   random bit generator. */

class engine
{
public:
  using result_type = uint64_t;

  /* An engine whose state is derived from `seed' by SplitMix64. */
  constexpr explicit engine(uint64_t seed = 0);

  static constexpr result_type min();
  static constexpr result_type max();

  /* Return the next value. */
  result_type operator()();

private:
  std::array<uint64_t, 4> state = {};
};

/* The global seed, together with the number of times it has been set, so a
   thread can tell when its engine is out of date. */

struct seed_state
{
  uint64_t seed = 0;
  uint64_t generation = 0;
};

/* The state of the calling thread's source of randomness. */

struct thread_state
{
  engine source;
  uint64_t bits = 0;		/* Unused bits of the last draw. */
  int remaining = 0;		/* Number of unused bits. */
  uint64_t generation = 0;	/* Generation of the seed in use. */
};

/* The SplitMix64 output function, a bijection that mixes its argument's
   bits thoroughly. */
constexpr uint64_t mix(uint64_t x);

/* Return the current global seed, choosing one if none has been set. */
seed_state current_seed();

/* Restart the calling thread's engine for trial `k' of a simulation. The
   sequence it then produces depends only on `s' and `k'. */
void start_trial(const seed_state& s, uint64_t k);

/* Return 64 random bits. */
uint64_t next();

/* Return a random bit. */
bool next_bit();

/* Return a value uniformly distributed on [0, n); `n' must be positive. */
uint64_t uniform(uint64_t n);

/* Shuffle [first, last) uniformly. */
template<std::random_access_iterator Iterator>
  void shuffle(Iterator first, Iterator last);

/* Save the calling thread's state, and restore it on destruction. */

class preserve_state
{
public:
  preserve_state();
  preserve_state(const preserve_state&) = delete;
  preserve_state& operator=(const preserve_state&) = delete;
  ~preserve_state();

private:
  thread_state saved;
};

} /* namespace internal */

} /* namespace hap */

/* Implementation. */

constexpr uint64_t
hap::internal::mix(uint64_t x)
{
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
  x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
  return x ^ (x >> 31);
}

/* SplitMix64 never yields the same value twice in four steps, so the state
   can't be all zero. */

constexpr
hap::internal::engine::engine(uint64_t seed)
{
  for (auto& word : state) {
    seed += 0x9e3779b97f4a7c15;
    word = mix(seed);
  }
}

constexpr auto
hap::internal::engine::min() -> result_type
{
  return 0;
}

constexpr auto
hap::internal::engine::max() -> result_type
{
  return ~result_type{0};
}

/* Fisher-Yates, using `uniform' rather than std::shuffle so that the result
   is the same with every standard library. */

template<std::random_access_iterator Iterator>
void
hap::internal::shuffle(Iterator first, Iterator last)
{
  using diff_t = std::iter_difference_t<Iterator>;

  for (auto n = last - first; n > 1; --n) {
    const auto k = static_cast<diff_t>(uniform(static_cast<uint64_t>(n)));
    std::ranges::iter_swap(first + (n - 1), first + k);
  }
}

#endif /* HAP_RANDOM_H_ */
