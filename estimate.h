/* hap/estimate.h -*- mode: c++ -*-
   Copyright (c) 2026 Derek B. Clegg
   All rights reserved. */

#ifndef HAP_ESTIMATE_H_
#define HAP_ESTIMATE_H_

/* Estimate the probability of an event by simulation. A scenario is a
   function that plays out one trial and returns true if the event
   occurred:

     hap::estimator e;
     auto r = e.estimate(1'000'000, []
     {
       hap::dice d{2};
       return d.sum() == 7;
     });

   To spread the trials over several threads, construct the estimator
   with a thread count, such as `hap::estimator{4}' or
   `hap::estimator{hap::estimator::max_threads}'.

   Trial k draws from a sequence determined only by the global seed and by
   k, so for a given seed the result is the same however many threads are
   used. Since trials may run on several threads at once, a scenario must
   create the objects it uses and must not change anything shared between
   trials. */

#include <cstdint>
#include <memory>
#include <type_traits>

namespace hap {

class estimator
{
public:
  class result;
  class scenario;

  /* Pass to `estimator(unsigned int)' to use the max number of threads
     allowed on the system. */
  static constexpr unsigned int max_threads = static_cast<unsigned int>(-1);

  /* An estimator that runs on a single thread. */
  estimator() = default;

  /* An estimator that runs on `threads' threads, or on as many as the
     hardware supports if `threads' is `max_threads'. A count of 0 is
     treated as 1. */
  explicit estimator(unsigned int threads);

  /* Return the number of threads this estimator uses. */
  unsigned int thread_count() const;

  /* Run `count' trials of `scenario' and estimate the probability that it
     returns true. `count' must be nonzero. */
  result estimate(uint64_t count, scenario scenario) const;

private:
  unsigned int thread_count_ = 1;
};

/* The outcome of a simulation. */

class estimator::result
{
public:
  result() = default;

  uint64_t trials = 0;		/* The number of trials run. */
  uint64_t successes = 0;	/* The number of successful trials. */
  double probability = 0;	/* Estimated probability of success. */
  double standard_error = 0;	/* Standard error of the estimate. */

private:
  friend class estimator;
  result(uint64_t, uint64_t);
};

/* A scenario, created implicitly from a lambda, a function object, or a
   function. Like std::function_ref<bool() const> (which can replace it once
   it's available), it refers to the scenario without owning it, so the
   scenario must outlive it; passing a scenario directly to estimate()
   always satisfies this. */

class estimator::scenario
{
public:
  /* Create a scenario using `f', which must take no arguments, return a
     value convertible to bool, and not modify its captures. */
  template<typename F>
    requires (!std::is_same_v<std::remove_cvref_t<F>, scenario>)
      scenario(const F& f);

  /* Run one trial and return true on success. */
  bool operator()() const;

private:
  /* What the scenario refers to: either a function object or a function. */
  union target
  {
    const void* object;
    bool (*function)();
  };
  target target_ = {};
  bool (*call_)(target) = nullptr;	/* Calls `target_'. */
};

} /* namespace hap */

/* Implementation. */

template<typename F>
  requires (!std::is_same_v<std::remove_cvref_t<F>, hap::estimator::scenario>)
hap::estimator::scenario::scenario(const F& f)
{
  static_assert(std::is_invocable_r_v<bool, const F&>,
    "A scenario should take no arguments, return bool, and must not "
    "modify its captures");

  if constexpr (std::is_function_v<F>) {
    static_assert(std::is_convertible_v<F*, bool (*)()>,
      "A function used as a scenario must return bool");
    target_.function = &f;
    call_ = [](target t) { return t.function(); };
  } else {
    target_.object = std::addressof(f);
    call_ = [](target t) -> bool {
      return (*static_cast<const F*>(t.object))();
    };
  }
}

inline bool
hap::estimator::scenario::operator()() const
{
  return call_(target_);
}

#endif /* HAP_ESTIMATE_H_ */
