// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AnySharedPointer.h"

using namespace autowiring;

AnySharedPointer::AnySharedPointer(AnySharedPointer&& rhs) :
  m_ti(rhs.m_ti),
  m_ptr(std::move(rhs.m_ptr))
{}

AnySharedPointer::~AnySharedPointer(void) {
}

static_assert(sizeof(AnySharedPointerT<int>) == sizeof(AnySharedPointer), "AnySharedPointer realization cannot have members");
static_assert(!std::is_polymorphic<AnySharedPointer>::value, "The shared pointer cannot be polymorphic, this prevents the root type from being aliased correctly");
