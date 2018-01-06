// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/ContextMember.h>
#include <autowiring/TypeUnifier.h>
#include TYPE_TRAITS_HEADER

// Simple ContextMember class:
class SimpleObject:
  public ContextMember
{
public:
  SimpleObject(void):
    zero(0),
    one(1)
  {}

  ~SimpleObject(void) {
  }

  int zero;
  int one;
};

static_assert(
  std::is_same<
    SimpleObject,
    typename autowiring::SelectTypeUnifier<SimpleObject>::type
  >::value,
  "The SimpleObject was incorrectly identified as needing a type unifier"
);
