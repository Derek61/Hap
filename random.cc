/* hap/random.cc -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#include <hap/random.h>
#include <nyx/int128.h>
#include <nyx/utility.h>
#include <atomic>
#include <mutex>
#include <random>

namespace {

using hap::internal::engine;
using hap::internal::mix;
using hap::internal::seed_state;
using hap::internal::thread_state;

/* Constants that keep the sequences of threads and of trials apart. */

constexpr uint64_t thread_domain = 0x7468726561647321;
constexpr uint64_t trial_domain = 0x747269616c732121;

/* The global seed. `generation' is 0 until a seed has been chosen, and is
   incremented each time the seed is set. A seed is stored before its
   generation is published, so a thread that sees a generation also sees its
   seed (or a later one, which only restarts its engine once more). */

std::atomic<uint64_t> global_seed{0};
std::atomic<uint64_t> generation{0};

/* Ensures that only one thread chooses a seed when none has been set. */

std::once_flag seed_chosen;

/* The number of threads that have drawn from Hap. */

std::atomic<uint64_t> thread_count{0};

thread_local thread_state state_of_thread;

/* Return a seed for sequence `k' in `domain' under the global seed `s'. */

constexpr uint64_t
key(uint64_t s, uint64_t domain, uint64_t k)
{
  return mix(s ^ mix(domain ^ mix(k)));
}

/* Restart the calling thread's engine from the current global seed. A
   thread keeps its number for life, so a thread's sequence depends only on
   the seed and on the order in which threads first drew from Hap. */

void
reseed(thread_state& st)
{
  thread_local const uint64_t ordinal = thread_count++;

  const auto s = hap::internal::current_seed();
  st.source = engine{key(s.seed, thread_domain, ordinal)};
  st.bits = 0;
  st.remaining = 0;
  st.generation = s.generation;
}

/* Return the calling thread's state, restarting its engine if the global
   seed has changed since it was last used. */

thread_state&
state()
{
  auto& st = state_of_thread;
  const auto g = generation.load(std::memory_order_acquire);
  if (g == 0 || g != st.generation)
    reseed(st);
  return st;
}

} /* namespace */

void
hap::seed(uint64_t s)
{
  global_seed.store(s, std::memory_order_relaxed);
  generation.fetch_add(1, std::memory_order_release);
}

uint64_t
hap::seed()
{
  return internal::current_seed().seed;
}

auto
hap::internal::engine::operator()() -> result_type
{
  auto rotl = [](uint64_t x, int k) { return (x << k) | (x >> (64 - k)); };

  const auto result = rotl(state[1] * 5, 7) * 9;
  const auto t = state[1] << 17;
  state[2] ^= state[0];
  state[3] ^= state[1];
  state[1] ^= state[2];
  state[0] ^= state[3];
  state[2] ^= t;
  state[3] = rotl(state[3], 45);
  return result;
}

auto
hap::internal::current_seed() -> seed_state
{
  std::call_once(seed_chosen, []
  {
    if (generation.load(std::memory_order_acquire) == 0) {
      std::random_device device;
      const auto s = (uint64_t{device()} << 32) | device();
      global_seed.store(s, std::memory_order_relaxed);
      generation.store(1, std::memory_order_release);
    }
  });
  const auto g = generation.load(std::memory_order_acquire);
  return {global_seed.load(std::memory_order_relaxed), g};
}

void
hap::internal::start_trial(const seed_state& s, uint64_t k)
{
  auto& st = state_of_thread;
  st.source = engine{key(s.seed, trial_domain, k)};
  st.bits = 0;
  st.remaining = 0;
  st.generation = s.generation;
}

uint64_t
hap::internal::next()
{
  return state().source();
}

bool
hap::internal::next_bit()
{
  auto& st = state();
  if (st.remaining == 0) {
    st.bits = st.source();
    st.remaining = 64;
  }
  const bool value = (st.bits & 1) != 0;
  st.bits >>= 1;
  --st.remaining;
  return value;
}

/* Lemire's method: the high word of x * n is uniform on [0, n) once the
   values of x whose low word falls below 2^64 mod n are rejected. */

uint64_t
hap::internal::uniform(uint64_t n)
{
  NYX_PRECONDITION(n > 0);
  auto& st = state();
  auto m = nyx::uint128{st.source()} * n;
  if (auto low = static_cast<uint64_t>(m); low < n) {
    const auto threshold = (0 - n) % n;
    while (low < threshold) {
      m = nyx::uint128{st.source()} * n;
      low = static_cast<uint64_t>(m);
    }
  }
  return static_cast<uint64_t>(m >> 64);
}

hap::internal::preserve_state::preserve_state()
  : saved{state_of_thread}
{
}

hap::internal::preserve_state::~preserve_state()
{
  state_of_thread = saved;
}
