#pragma once
#include RVALUE_HEADER

template<class Fn>
struct AtExit {
  AtExit(Fn&& fn) :
    fn(std::move(fn))
  {}

  ~AtExit(void) {
    fn();
  }

  Fn fn;
};

template<class Fn>
AtExit<Fn> MakeAtExit(Fn&& fn) {
  return AtExit<Fn>(std::forward<Fn>(fn));
}
