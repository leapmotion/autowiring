// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AnySharedPointer.h"

AnySharedPointer::AnySharedPointer(void) {
  new (m_space) SharedPointerSlot;
}

AnySharedPointer::AnySharedPointer(AnySharedPointer&& rhs)
{
  new (m_space) SharedPointerSlot(std::move(*rhs.slot()));
}

AnySharedPointer::AnySharedPointer(const AnySharedPointer& rhs) {
  new (m_space) SharedPointerSlot(*rhs.slot());
}

AnySharedPointer::AnySharedPointer(const SharedPointerSlot&& rhs) {
  new (m_space) SharedPointerSlot(std::move(rhs));
}

AnySharedPointer::AnySharedPointer(const SharedPointerSlot& rhs){
  new (m_space) SharedPointerSlot(rhs);
}


AnySharedPointer::~AnySharedPointer(void) {
  // Pass control to the *real* destructor:
  slot()->~SharedPointerSlot();
}

static_assert(sizeof(AnySharedPointerT<int>) == sizeof(AnySharedPointer), "AnySharedPointer realization cannot have members");
static_assert(!std::is_polymorphic<AnySharedPointer>::value, "The shared pointer cannot be polymorphic, this prevents the root type from being aliased correctly");
