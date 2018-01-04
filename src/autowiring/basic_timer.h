// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include <chrono>

template<class clock_t, class duration_t = typename clock_t::duration>
class basic_timer {
public:
  basic_timer(const typename clock_t::time_point& initial = clock_t::now()) : m_start(initial) {}

  //A convenient method to avoid having to use duration_cast all the time.
  duration_t now() const { return std::chrono::duration_cast<duration_t>(clock_t::now().time_since_epoch()); }
  duration_t elapsed() const { return std::chrono::duration_cast<duration_t>(clock_t::now() - m_start); }
  typename clock_t::time_point start_time() const { return m_start; }

  void start() { m_start = clock_t::now(); }
  duration_t mark() {
    const auto now = clock_t::now();
    const auto oldStart = m_start;
    m_start = now;
    return std::chrono::duration_cast<duration_t>(now - oldStart);
  }
private:
  typename clock_t::time_point m_start;
};

