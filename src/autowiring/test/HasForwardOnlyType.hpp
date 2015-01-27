// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/autowiring.h>

class MyForwardedType;

class HasForwardOnlyType {
public:
  HasForwardOnlyType(void);

  Autowired<MyForwardedType> forwarded;

  void AutoFilter(std::shared_ptr<MyForwardedType>& output);
};
