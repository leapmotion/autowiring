// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/autowiring.h>

class MyForwardedType;

class ForwardedOnlyBaseType {};

class HasForwardOnlyType:
  public ForwardedOnlyBaseType
{
public:
  HasForwardOnlyType(void);

  Autowired<MyForwardedType> forwarded;

  void AutoFilter(std::shared_ptr<MyForwardedType>& output);
};

/// <summary>
/// Injects the forward-only type into the passed context
/// </summary>
void InjectForwardOnlyType(CoreContext& ctxt);
