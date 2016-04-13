// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutowirableSlot.h"
#include "autowiring_error.h"
#include "CoreContext.h"
#include <cassert>
#include <stdexcept>

using namespace autowiring;
using namespace std;

void DeferrableAutowiring::Handler::operator()() {
  parent.m_ptr = memo.m_value;

  // Move to a stack-allocated field under lock so we can free outside of the lock
  // Also has the side effect of releasing _all_ memory, as opposed to `clear`, which
  // only sets the size to zero.
  auto autowired_notifications =
    (
      std::lock_guard<spin_lock>{ parent.m_lock },
      std::move(parent.m_autowired_notifications)
    );
}

DeferrableAutowiring::DeferrableAutowiring(const DeferrableAutowiring& rhs) :
  DeferrableAutowiring(AnySharedPointer{ rhs.m_ptr }, rhs.m_context.lock())
{}

DeferrableAutowiring::DeferrableAutowiring(AnySharedPointer&& witness, const std::shared_ptr<CoreContext>& context) :
  m_ptr(std::move(witness)),
  m_context(context)
{
  if (!context)
    return;

  // We need to know when the type itself becomes available:
  MemoEntry& memo = context->FindByType(witness.type(), false);
  registration_t reg =
    memo.onSatisfied += Handler{ *this, memo };

  if (!reg)
    // Invoked, do not need our notifications structure
    return;

  // Deferred notifications are extremely common.  Generally clients only need one or so, and resizing requires
  // that we perform an expensive memory reallocation, so let's pay a little memory in advance to help make the
  // most common use case pretty fast.
  m_autowired_notifications.reserve(2);
  m_autowired_notifications.emplace_back(std::move(reg));
}

DeferrableAutowiring::~DeferrableAutowiring(void) {
  // BUG:  The reset call does not strictly prohibit signal handlers defined on this type from
  // being invoked after the destruction of this type.  We must rely on external synchronization
  // strategies to prevent a use-after-free; in particular, users that are making use of
  // Autowired<T> on the stack may be subject to race conditions under teardown.  There is no
  // general solution to this problem that does not introduce the possibility of a deadlock.
  //
  // The only solution available may be to trigger a fatal exception when a cancellation race
  // is detected.
  reset();
}

void DeferrableAutowiring::reset(void) {
  // Local versions of the members we are clearing out
  std::weak_ptr<CoreContext> contextWeak;
  AnySharedPointer ptr;
  std::vector<registration_t> autowired_notifications;

  // Move and handle resources under lock
  {
    std::lock_guard<spin_lock> lk{ m_lock };
    autowired_notifications = std::move(m_autowired_notifications);
    ptr = std::move(m_ptr);
    contextWeak = std::move(m_context);
    m_context.reset();
  }

  std::shared_ptr<CoreContext> context = contextWeak.lock();
  if (!context)
    // Nothing to do, context is already gone.  No chance of a signal being
    // asserted now.
    return;

  // Clear out any registered signal handlers, context is still valid:
  for (auto& notification : autowired_notifications)
    *notification.owner -= notification;
}
