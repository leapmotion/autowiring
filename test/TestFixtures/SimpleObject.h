#pragma once
#include "ContextMember.h"
#include "TypeUnifier.h"
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
    typename SelectTypeUnifier<SimpleObject>::type
  >::value,
  "The SimpleObject was incorrectly identified as needing a type unifier"
);