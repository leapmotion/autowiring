// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "once.h"

using namespace autowiring;

once::once(once&& rhs) :
  m_state(rhs.m_state),
  m_fns(std::move(rhs.m_fns))
{
  rhs.m_fns.clear();
}

bool once::operator-=(registration_t& rhs) {
  if (get())
    // No unregistration supported after the flag is set--we assume a cancellation race
    return false;

  // Double-check under lock
  std::lock_guard<autowiring::spin_lock> lk(m_spin);
  if (get())
    return false;

  for (size_t i = m_fns.size(); i--;)
    if (m_fns[i].get() == rhs.pobj) {
      m_fns[i] = std::move(m_fns.back());
      m_fns.pop_back();
      return true;
    }
  throw std::runtime_error("Attempted to unregister a once signal that was not created on this object.");
}

void once::signal(void) {
  // Standard lock double-check
  if (get())
    return;

  {
    std::lock_guard<autowiring::spin_lock> lk(m_spin);
    if (get())
      return;

    // We now have ownership of our vector.  Nobody else will append to it
    // once this flag is set.
    m_state = state::signalling;
  }

  // Invoke all registered handlers outside of the lock:
  for (auto& cur : m_fns)
    (*cur)();

  // Completely clear the vector, releasing absolutely all memory.  We will never need it again.
  m_state = state::signalled;
  m_fns = std::vector<std::unique_ptr<detail::callable_base>>{};
}

void once::operator=(bool rhs) {
  if (!rhs)
    throw std::runtime_error("Cannot set a once flag to any value other than true");
  signal();
}
