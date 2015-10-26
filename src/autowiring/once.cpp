// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "once.h"

using namespace autowiring;

void once::signal(void) {
  // Standard lock double-check
  if (flag)
    return;

  std::vector<std::unique_ptr<detail::callable_base>> fns;
  {
    std::lock_guard<autowiring::spin_lock> lk(m_spin);
    if (flag)
      return;
    flag = true;
    fns = std::move(m_fns);
    m_fns.clear();
  }

  // Invoke all registered handlers outside of the lock:
  for (auto& cur : fns)
    (*cur)();
}

void once::operator=(bool rhs) {
  if (!rhs)
    throw std::runtime_error("Cannot set a once flag to any value other than true");
  signal();
}
